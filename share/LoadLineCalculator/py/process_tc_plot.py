def parse_args():
  import argparse
  prog = argparse.ArgumentParser(
    prog = "process_tc_plot_hybrid",
    description = "Hybrid A* Pathfinder & Manual Point Digitizer for vacuum tube transfer curves."
  )
  prog.add_argument(
    "--input-file",
    "-i",
    dest = "input_path",
    required = True
  )
  prog.add_argument(
    "--output-file",
    "-o",
    dest = "output_path",
    default = "extracted_curves.csv"
  )
  prog.add_argument(
    "--threshold",
    "-t",
    dest = "threshold",
    type = int,
    default = 180,
    help = "Grayscale threshold value (0-255) for binarization."
  )
  prog.add_argument(
    "--dot-size",
    "-d",
    dest = "dot_size",
    type = int,
    default = 1,
    choices = [1, 3],
    help = "Point size rendered on canvas in Manual Mode (1 or 3 px)."
  )
  prog.add_argument(
    "--show-binary",
    "-s",
    dest = "show_binary",
    default = False,
    action = "store_true"
  )
  args = prog.parse_args()
  return args

def resample_and_smooth_path(path, spacing=5, window_size=3):
  """Resamples a pixel path to uniform spatial distance, then applies sliding average."""
  if len(path) < 2:
    return path
    
  resampled = [path[0]]
  accumulated_dist = 0.0
  
  for i in range(1, len(path)):
    p1 = np.array(path[i-1])
    p2 = np.array(path[i])
    seg_dist = np.linalg.norm(p2 - p1)
    
    if seg_dist == 0:
      continue
      
    accumulated_dist += seg_dist
    
    while accumulated_dist >= spacing:
      overshoot = accumulated_dist - spacing
      t = 1.0 - (overshoot / seg_dist)
      new_pt = p1 + t * (p2 - p1)
      resampled.append((new_pt[0], new_pt[1]))
      
      p1 = new_pt
      seg_dist = np.linalg.norm(p2 - p1)
      accumulated_dist = seg_dist
      
  resampled.append(path[-1])
  
  if len(resampled) < window_size:
    return [(int(round(p[0])), int(round(p[1]))) for p in resampled]
    
  smoothed = []
  half_w = window_size // 2
  for i in range(len(resampled)):
    start_idx = max(0, i - half_w)
    end_idx = min(len(resampled), i + half_w + 1)
    subset = resampled[start_idx:end_idx]
    
    avg_x = sum(p[0] for p in subset) / len(subset)
    avg_y = sum(p[1] for p in subset) / len(subset)
    smoothed.append((int(round(avg_x)), int(round(avg_y))))
    
  return smoothed

def snap_to_nearest_ink(binary_img, dist_map, point, search_radius=7):
  """Snaps coordinate to highest-value ink pixel in a local radius."""
  x, y = point
  h, w = binary_img.shape
  
  best_pt = point
  max_val = -1
  
  y1, y2 = max(0, y - search_radius), min(h, y + search_radius + 1)
  x1, x2 = max(0, x - search_radius), min(w, x + search_radius + 1)
  
  for ny in range(y1, y2):
    for nx in range(x1, x2):
      if binary_img[ny, nx] > 0:
        val = dist_map[ny, nx]
        if val > max_val:
          max_val = val
          best_pt = (nx, ny)
          
  return best_pt

if __name__ == "__main__":
  import cv2
  import numpy as np
  import heapq
  import csv
  
  args = parse_args()
  print(f"Using input file: {args.input_path}")
  print(f"Using output file: {args.output_path}")

  # Load image
  img = cv2.imread(args.input_path)
  if img is None:
    print("Error: Could not load image.")
    exit(1)
      
  gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
  _, binary = cv2.threshold(gray, args.threshold, 255, cv2.THRESH_BINARY_INV)
  
  if (args.show_binary):
    print("==== Pausing to show binary ====")
    print("Press ESC or 'c' to close and continue")
    exited = False
    cv2.imshow("Binary Transform", binary)
    while (not exited):
      exit_key = cv2.waitKey(20) & 0xFF
      if exit_key == 27 or exit_key == ord('c'):
        cv2.destroyAllWindows()
        exited = True
      
    print("==== Continuing ====")

  dist_transform = cv2.distanceTransform(binary, cv2.DIST_L2, 3)
  max_dist = np.max(dist_transform)

  def directional_astar(binary_img, dist_map, start, end):
    """Directional A* optimized for steep knees and grid lines."""
    h, w = binary_img.shape
    dirs = [(0,1), (1,0), (0,-1), (-1,0), (1,1), (1,-1), (-1,1), (-1,-1)]
    pq = [(0, start[0], start[1], 0, 0)]
    
    costs = {}
    parent = {}
    costs[(start[0], start[1], 0, 0)] = 0
    
    while pq:
      cost, x, y, pdx, pdy = heapq.heappop(pq)
      
      if (x, y) == end:
        path = []
        curr = (x, y, pdx, pdy)
        while curr in parent:
          path.append((curr[0], curr[1]))
          curr = parent[curr]
        path.append(start)
        return path[::-1]
      
      for dx, dy in dirs:
        nx, ny = x + dx, y + dy
        
        if 0 <= nx < w and 0 <= ny < h:
          if binary_img[ny, nx] == 0:
            continue
          
          step_cost = 1.414 if (dx != 0 and dy != 0) else 1.0
          
          if pdx != 0 or pdy != 0:
            dot_product = (dx * pdx + dy * pdy) / ((dx**2 + dy**2)**0.5 * (pdx**2 + pdy**2)**0.5)
            turn_penalty = (1.0 - dot_product) * 4.0  
          else:
            turn_penalty = 0.0
            
          ridge_penalty = (max_dist - dist_map[ny, nx]) * 10.0 
          h_cost = abs(nx - end[0]) + abs(ny - end[1])
          
          new_cost = cost + step_cost + turn_penalty + ridge_penalty
          state = (nx, ny, dx, dy)
          
          if state not in costs or new_cost < costs[state]:
            costs[state] = new_cost
            parent[state] = (x, y, pdx, pdy)
            heapq.heappush(pq, (new_cost + h_cost, nx, ny, dx, dy))
                    
    return []

  # Global State Variables
  mode = "CALIBRATION"
  tracing_submode = "ASTAR"  # "ASTAR" or "MANUAL"
  calib_points = []
  calib_data = {}
  
  points = []
  extracted_dataset = []
  
  # Undo/Resample State Tracking
  canvas_history = []  # Stack of canvas snapshots before each curve
  point_counts = []   # Track how many points each curve added for easy rollback
  
  canvas_before_waypoints = None
  last_raw_path = None
  last_vg_val = None

  canvas = img.copy()
  zoom_radius = 20

  def px_to_units(px, py):
    x_ratio = (px - calib_data['origin_px'][0]) / (calib_data['x_max_px'][0] - calib_data['origin_px'][0])
    y_ratio = (calib_data['origin_px'][1] - py) / (calib_data['origin_px'][1] - calib_data['y_max_px'][1])
    
    va = calib_data['x_min_val'] + x_ratio * (calib_data['x_max_val'] - calib_data['x_min_val'])
    ia = calib_data['y_min_val'] + y_ratio * (calib_data['y_max_val'] - calib_data['y_min_val'])
    return va, ia

  def draw_point(target_canvas, pt, color=(0, 255, 0)):
    """Draws a point with configured size in manual mode."""
    x, y = pt
    if args.dot_size == 1:
      target_canvas[y, x] = color
    else:
      cv2.circle(target_canvas, (x, y), 1, color, -1)

  def process_and_draw_astar_curve(raw_path, vg_val, spacing_px):
    """Resamples path, appends dataset, and draws green points."""
    global canvas, extracted_dataset, point_counts
    
    path = resample_and_smooth_path(raw_path, spacing=spacing_px, window_size=3)
    point_counts.append(len(path))
    
    for px, py in path:
      cv2.circle(canvas, (px, py), 2, (0, 255, 0), -1)
      va, ia = px_to_units(px, py)
      extracted_dataset.append({
        'Vg': vg_val,
        'Va': round(va, 3),
        'Ia': round(ia, 4),
        'px': px,
        'py': py
      })
      
    cv2.imshow("Curve Tracer", canvas)
    print(f"Saved A* Curve ({len(path)} pts) for Vg = {vg_val}.")

  def mouse_event(event, x, y, flags, params):
    global points, canvas, calib_points, mode, calib_data
    global last_raw_path, last_vg_val, canvas_before_waypoints
    global extracted_dataset, canvas_history, point_counts
    
    # Magnifying Glass View
    if event in (cv2.EVENT_MOUSEMOVE, cv2.EVENT_LBUTTONDOWN, cv2.EVENT_RBUTTONDOWN):
      ch, cw = canvas.shape[:2]
      y1, y2 = max(0, y - zoom_radius), min(ch, y + zoom_radius)
      x1, x2 = max(0, x - zoom_radius), min(cw, x + zoom_radius)
      
      crop = canvas[y1:y2, x1:x2]
      if crop.size > 0:
        zoom_view = np.zeros((zoom_radius*2, zoom_radius*2, 3), dtype=np.uint8)
        zy1 = zoom_radius - (y - y1)
        zy2 = zy1 + (y2 - y1)
        zx1 = zoom_radius - (x - x1)
        zx2 = zx1 + (x2 - x1)
        zoom_view[zy1:zy2, zx1:zx2] = crop
        
        zoom_scaled = cv2.resize(zoom_view, (400, 400), interpolation=cv2.INTER_NEAREST)
        cv2.line(zoom_scaled, (200, 0), (200, 400), (0, 0, 255), 1)
        cv2.line(zoom_scaled, (0, 200), (400, 200), (0, 0, 255), 1)
        cv2.imshow("Zoom", zoom_scaled)

    # Calibration Sequence
    if mode == "CALIBRATION":
      if event == cv2.EVENT_LBUTTONDOWN:
        calib_points.append((x, y))
        temp_canvas = canvas.copy()
        for pt in calib_points:
          cv2.circle(temp_canvas, pt, 4, (255, 0, 0), -1)
        cv2.imshow("Curve Tracer", temp_canvas)
        
        if len(calib_points) == 1:
          print("Click 2: Top-Left Y-Axis Max point")
        elif len(calib_points) == 2:
          print("Click 3: Bottom-Right X-Axis Max point")
        elif len(calib_points) == 3:
          print("\n--- AXIS CALIBRATION INPUTS ---")
          calib_data['origin_px'] = calib_points[0]
          calib_data['y_max_px'] = calib_points[1]
          calib_data['x_max_px'] = calib_points[2]
          
          calib_data['x_min_val'] = float(input("Enter X-Axis Origin value (Plate Volts, Va): "))
          calib_data['x_max_val'] = float(input("Enter X-Axis Max value (Plate Volts, Va): "))
          calib_data['y_min_val'] = float(input("Enter Y-Axis Origin value (Plate Current, Ia): "))
          calib_data['y_max_val'] = float(input("Enter Y-Axis Max value (Plate Current, Ia): "))
          
          mode = "TRACING"
          canvas = img.copy()
          cv2.imshow("Curve Tracer", canvas)
          
          print("\nCalibration Complete! Default Mode: [A* PATHFINDING]")
          print("Controls: Left-Click waypoints | Right-Click finish | 'm' toggle Manual/A* | 'r' undo last curve\n")

    # Tracing Handlers
    elif mode == "TRACING":
      if event == cv2.EVENT_LBUTTONDOWN:
        if len(points) == 0:
          canvas_before_waypoints = canvas.copy()
          
        points.append((x, y))
        if tracing_submode == "ASTAR":
          cv2.circle(canvas, (x, y), 3, (0, 0, 255), -1)
        else:
          draw_point(canvas, (x, y), color=(0, 0, 255))
        cv2.imshow("Curve Tracer", canvas)

      elif event == cv2.EVENT_RBUTTONDOWN:
        if not points:
          print("No points placed for this curve!")
          return

        # Handle A* Pathfinder Finalization
        if tracing_submode == "ASTAR":
          if len(points) < 2:
            print("A* Mode requires at least 2 waypoints!")
            return
            
          full_raw_path = []
          success = True
          
          for i in range(len(points) - 1):
            p_start = snap_to_nearest_ink(binary, dist_transform, points[i])
            p_end = snap_to_nearest_ink(binary, dist_transform, points[i+1])
            
            segment = directional_astar(binary, dist_transform, p_start, p_end)
            if not segment:
              print(f"FAILED between Waypoint {i+1} and Waypoint {i+2}.")
              success = False
              break
            full_raw_path.extend(segment if i == 0 else segment[1:])
          
          if success and full_raw_path:
            # Save clean state to canvas history for undo ('r')
            canvas_history.append(canvas_before_waypoints.copy())
            
            vg_val = float(input("Enter Grid Voltage (Vg) for this curve: "))
            spacing_px = float(input("Enter point spacing interval in pixels (default 5): ") or 5.0)
            
            canvas = canvas_before_waypoints.copy()
            last_raw_path = full_raw_path
            last_vg_val = vg_val
            
            process_and_draw_astar_curve(full_raw_path, vg_val, spacing_px)
          else:
            print("Resetting failed waypoints.")
            canvas = canvas_before_waypoints.copy()
            cv2.imshow("Curve Tracer", canvas)

        # Handle Manual Mode Finalization
        else:
          canvas_history.append(canvas_before_waypoints.copy())
          vg_val = float(input("Enter Grid Voltage (Vg) for this curve: "))
          
          canvas = canvas_before_waypoints.copy()
          point_counts.append(len(points))
          
          for px, py in points:
            draw_point(canvas, (px, py), color=(0, 255, 0))
            va, ia = px_to_units(px, py)
            extracted_dataset.append({
              'Vg': vg_val,
              'Va': round(va, 3),
              'Ia': round(ia, 4),
              'px': px,
              'py': py
            })
            
          cv2.imshow("Curve Tracer", canvas)
          print(f"Saved Manual Curve ({len(points)} pts) for Vg = {vg_val}.")

        points = []

  # Main Window Initialization
  cv2.namedWindow("Curve Tracer")
  cv2.namedWindow("Zoom")
  cv2.setMouseCallback("Curve Tracer", mouse_event)
  cv2.imshow("Curve Tracer", canvas)
  
  print("=== CALIBRATION MODE ===")
  print("Click 1: Bottom-Left Origin (X_min, Y_min)")
  
  while True:
    key = cv2.waitKey(20) & 0xFF
    
    # 'm' key toggles between A* Pathfinder and Manual mode
    if key == ord('m'):
      if mode == "TRACING":
        if tracing_submode == "ASTAR":
          tracing_submode = "MANUAL"
          print("\n>>> Switched to MANUAL TRACING MODE. (Left-click points, Right-click save)")
        else:
          tracing_submode = "ASTAR"
          print("\n>>> Switched to AUTOMATIC A* PATHFINDER MODE.")
        points = []

    # 'r' key removes/undoes the last committed curve
    if key == ord('r'):
      if not canvas_history or not point_counts:
        print("\nNo curves available to remove!")
      else:
        # Revert canvas to image state before the last curve
        canvas = canvas_history.pop()
        pts_to_remove = point_counts.pop()
        
        # Remove points from dataset
        extracted_dataset = extracted_dataset[:-pts_to_remove]
        
        cv2.imshow("Curve Tracer", canvas)
        print(f"\n--- REMOVED LAST CURVE ({pts_to_remove} points erased) ---")
        print(f"Total dataset now stands at {len(extracted_dataset)} points.\n")

    # 'b' key re-samples last A* trace
    if key == ord('b'):
      if last_raw_path is None or not canvas_history:
        print("\nNo previous A* curve to re-sample!")
      else:
        print(f"\n--- RE-SAMPLING LAST A* CURVE (Vg = {last_vg_val}) ---")
        new_spacing = float(input("Enter NEW point spacing interval in pixels: "))
        
        # Revert to last saved clean state
        canvas = canvas_history[-1].copy()
        pts_to_remove = point_counts.pop()
        extracted_dataset = extracted_dataset[:-pts_to_remove]
        
        process_and_draw_astar_curve(last_raw_path, last_vg_val, new_spacing)

    # ESC or 'q' to quit and save
    if key == 27 or key == ord('q'):
      break

  cv2.destroyAllWindows()

  # Output CSV
  if extracted_dataset:
    print(f"\nWriting {len(extracted_dataset)} total points to {args.output_path}...")
    with open(args.output_path, mode='w', newline='') as f:
      writer = csv.DictWriter(f, fieldnames=['Vg', 'Va', 'Ia', 'px', 'py'])
      writer.writeheader()
      writer.writerows(extracted_dataset)
    print("Data export complete!")
  else:
    print("No data collected. Exiting.")