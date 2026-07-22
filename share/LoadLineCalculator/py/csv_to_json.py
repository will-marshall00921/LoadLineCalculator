# LoadLineCalculator -- csv_to_json.py
# Author: Will Marshall
# Created: 7/20/2026, 9:43:41 PM

def parse_args():
  import argparse
  parser = argparse.ArgumentParser(
    prog = "csv_to_json",
    description = "Convert processed CSV tube curves to their JSON format"
  )
  parser.add_argument(
    "--input-file",
    "-i",
    required = True,
    dest = "input_path"
  )
  parser.add_argument(
    "--target-file",
    "-t",
    required = True,
    dest = "target_path"
  )
  parser.add_argument(
    "--units-prefix",
    "-p",
    required = True,
    dest = "units_prefix"
  )
  parser.add_argument(
    "--show-data",
    "-s",
    default = False,
    dest = "show_data",
    action = "store_true"
  )
  args = parser.parse_args()
  return args

import json
def load_json(path):
  with open(path, "r") as fi:
    data = json.load(fi)
  return data

def write_json(path, data):
  with open(path, "w") as fo:
    json.dump(data, fo, indent=2)

def read_csv(path):
  import numpy as np
  raw_data = np.genfromtxt(
    path, 
    dtype=np.float64, 
    delimiter = ",", 
    skip_header = 1, 
    usecols = [0,1,2]
  )
  data = {}
  curve_count = 0
  for i in range(raw_data.shape[0]):
    if raw_data[i, 0] in data.keys():
      data[raw_data[i,0]]["plate_voltages"] += [raw_data[i, 1]]
      data[raw_data[i,0]]["plate_currents"] += [raw_data[i, 2]] 
    else:
      data[raw_data[i,0]] = {
        "grid_voltage": raw_data[i, 0],
        "plate_voltages": [raw_data[i, 1]],
        "plate_currents": [raw_data[i, 2]]
      }
  obj_list = []
  for key in data.keys():
    data[key]["point_count"] = f"{len(data[key]["plate_voltages"]):d}"
    obj_list += [data[key]]
  return obj_list

if __name__ == "__main__":
  args = parse_args()
  print(f"Current units set to: {args.units_prefix}A")
  print(f"Reading CSV: {args.input_path}")
  obj_list = read_csv(args.input_path)
  print(f"Found {len(obj_list)} curves!")
  print(f"Reading JSON: {args.target_path}")
  json_data = load_json(args.target_path)
  if "curves" in json_data.keys():
    print(f"Removing {len(obj_list)} curves from {args.target_path}")
    json_data["curves"] = []
    
  print(f"JSON values loaded!")
  json_data["curves"] = {
    "count": f"{len(obj_list):d}",
    "plate_current_units": f"{args.units_prefix}A",
    "data": []
  }
  for obj in obj_list:
    json_data["curves"]["data"] += [obj]
  print(f"Writing JSON: {args.target_path}")
  write_json(args.target_path, json_data)
  print("File updated!")
  if (args.show_data):
    import matplotlib.pyplot as plt
    print("Plotting curves!")
    f, a = plt.subplots()
    for obj in obj_list:
      a.plot(
        obj["plate_voltages"],
        obj["plate_currents"],
        color = 'black',
        linestyle = '-'
      )
    plt.show()