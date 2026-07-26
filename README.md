# LoadLineCalculator
## About

Author: Will Marshall

LoadLineCalculator is a basic tube database and load-line calculator. It includes
digitized versions of datasheet parameters for several common vacuum tube triodes
and pentodes, allowing the user to quickly calculate a load-line for the common
tube models.

## Tube Files

Tube files are by default installed to the `%APPDATA%/Roaming/LoadLineCalculator/tubes`
directory. The tube files are stored in the JSON format to allow for the user to
easily add their own tubes. The top level JSON keys are given below.

Key            | Type   | Description
---------------|--------|------------
"name"         | string | The name of the tube model, i.e. "12AX7", "6V6", etc.
"type"         | string | Type of tube, allowed values are "Triode" and "Pentode".
"heater"       | object | The heater information for the tube.
"ratings"      | object | The maximum ratings for the tube.
"capacitances" | object | The interelectrode capacitances for the tube.
"curves"       | object | The digitized transfer curves for the tube.

### Heater Section

The heater section gives the voltage and current required for the operation of
the heater in the tube. The "heater" object JSON keys are given below.

Key                  | Type   | Description
---------------------|--------|------------
"vdc_parallel"       | number | The voltage required for the heater.
"vdc_parallel_units" | string | The units of the "vdc_parallel" value, i.e. "V".
"i_parallel"         | number | The current required for the heater.
"i_parallel_units"   | string | The units of the "i_parallel" value, i.e. "mA".

### Ratings Section

The ratings section gives various maximum design ratings for the tube. The
"ratings" object JSON keys are given below.

Key                | Type   | Description
-------------------|--------|------------
"max_v_a_k"        | number | The maximum allowed anode voltage.
"max_v_a_k_units"  | string | The units of the "max_v_a_k" value, i.e. "V".
"max_v_g2_k"       | number | The maximum allowed screen-grid voltage.
"max_v_g2_k_units" | string | The units of the "max_v_g2_k" value, i.e. "V".
"max_p_a"          | number | The maximum anode power dissipation.
"max_p_a_units"    | string | The units of the "max_p_a" value, i.e. "W".
"max_p_g2"         | number | The maximum screen-grid power dissipation.
"max_p_g2_units"   | string | The units of the "max_p_g2" value, i.e. "W".
"max_v_g1_k"       | number | The maximum allowed control-grid voltage.
"max_v_g1_k_units" | string | The units of the "max_v_g1_k" value, i.e. "V".
"min_v_g1_k"       | number | The minimum allowed control grid voltage.
"min_v_g1_k_units" | string | The units of the "min_v_g1_k" value, i.e. "V".
"max_v_k"          | number | The maximum allowed heater-to-cathode voltage.
"max_v_k_units"    | string | The units of the "max_v_k" value, i.e. "V".
"min_v_k"          | number | The minimum allowed heater-to-cathode voltage.
"min_v_k_units"    | string | The units of the "min_v_k" value, i.e. "V".

### Capacitances Section

The capacitances section provides the various interelectrode capacitances for 
the tube. The "capacitances" object JSON keys are given below.

Key            | Type   | Description
---------------|--------|------------
"c_g1_a"       | number | The control-grid to anode capacitance.
"c_g1_a_units" | string | The units of the "c_g1_a" value, i.e. "pF".
"c_g1_k"       | number | The control-grid to cathode capacitance.
"c_g1_k_units" | string | The units of the "c_g1_k" value, i.e. "pF".
"c_a_k"        | number | The anode to cathode capacitance.
"c_a_k_units"  | string | The units of the "c_a_k" value, i.e. "pF".

### Curves Section

The curves section provides the transfer curves for the various grid voltages
as digitized from the datasheet plot. The "curves" object JSON keys are given
below.

Key                   | Type          | Description
----------------------|---------------|------------
"count"               | string        | The number of curve objects contained in the "data" array, formatted into a string, i.e. "7".
"plate_current_units" | string        | The units of the current values in the curves, only "mA" is supported.
"data"                | array[object] | An array containing the individual curve objects, one for each grid voltage curve in the plot provided in the tube's datasheet.

#### Curve Objects

A curve object is a simple JSON object. The JSON keys for the curve object are
given below.

Key              | Type          | Description
-----------------|---------------|------------
"grid_voltage"   | number        | The grid voltage of the curve.
"point_count"    | string        | The number of points in the digitized curve, formatted as a string, i.e. "58".
"plate_voltages" | array[number] | The array of plate voltage point locations, sorted in ascending order.
"plate_currents" | array[number] | The array of plate currents, each corresponding to the plate voltage in with the same index in the "plate_voltages" array.

## Build & Install

The project is built with CMake (version 3.16 or greater). The project uses the
standard CMake configure command:
```
$ cmake -B <build/directory> -S <source/directory> -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<installation/directory>
```
where "<build/directory>" is the path to the desired build directory, "<source/directory>" is the path to the project root directory where this README resides,
and "<installation/directory>" is the path of the desired installation directory.

The build and installation is also handled with the default CMake command:
```
$ cmake --build <build/directory> --target install
```