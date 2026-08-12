
# ODD Engine

This repository implements an ODD-oriented definition language and a runtime environment for monitoring and restricting the functionality of an Automated Driving System (ADS) based on predefined Operational Design Domains (ODD).

## Overview

- **Ontology:** Knowledge structure based on OWL, defining attributes and relations within the ODD.
- **ODD Engine:** Evaluation of the current vehicle situation based on defined rules and
  restrictions, including the combination of rules into Guardrails. ODDs are defined in a
  human-readable YAML format, and the engine outputs restriction keys (e.g., ToR or MRM) to
  control ADS behavior.
- **Integration:** Modern C++17 implementation using smart pointers and standard library
  features, usable directly or through the Python bindings. A ROS2 node connects the engine
  with an existing ADS (e.g., from the ADP project at FH Aachen).

## Usage

- Define the ODD in a YAML file
- Create an ontology using [Protégé](https://github.com/protegeproject/protege) and export it as an RDF (.rdf) file
- Include the ODD engine in python or C++
- The engine outputs active restriction keys that the ADS can use to adjust driving behavior

## Dependencies

System packages:
```bash
sudo apt install build-essential cmake pkg-config librdf0 librdf0-dev
```

For the Python bindings you additionally need the Python headers:
```bash
sudo apt install python3-dev
```

`yaml-cpp` is fetched automatically by CMake (`FetchContent`), and
[CParse](https://github.com/cparse/cparse) is vendored under `include/cparse`
and compiled from source as part of the build — no separate clone or
`make release` step is required.

`pybind11` is only needed for the C++ build of the Python module. It is pulled
in automatically when you build through `pip` (see below); for a plain CMake
build, install it yourself:
```bash
pip install pybind11
```

## Install (C++)

Build the libraries:
```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

Then include it in a CMake project:
```cmake
find_package(odd_engine)
```

## Install (Python bindings)

The Python module is built through [scikit-build-core](https://github.com/scikit-build/scikit-build-core),
which drives the same CMake build. Install the system dependencies listed above
first, then, from the repository root:

```bash
pip install --user .
```

or 

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install .
```
Verify the install:
```bash
python3 -c "import oddengine; print(oddengine.ODDEngine)"
```

### Rebuilding after C++ changes

A normal install is a snapshot: the C++ sources are compiled once and copied
into your environment, so **changes to any `.cpp`/`.h` will not take effect
until you re-run `pip install --user .`**. The same applies to
`src/bindings.cpp` — newly exposed methods only appear after a reinstall.

For an editable install that recompiles automatically whenever an import
detects changed sources:

```bash
pip install --user scikit-build-core pybind11
pip install --user --no-build-isolation -Ceditable.rebuild=true -e .
```

Both flags are required. `--no-build-isolation` is what makes the build
dependencies (installed in the first line) visible at rebuild time, and
`editable.rebuild=true` is what enables the rebuild — a plain `pip install -e .`
installs a redirect to a *fixed* compiled module and will **not** pick up C++
changes on its own.

### Building the module without pip

A plain CMake build also produces the module, at
`build/oddengine.cpython-*.so`. It is only importable from `build/` unless you
add that directory to `PYTHONPATH`:

```bash
export PYTHONPATH=$PWD/build:$PYTHONPATH
```



## C++ example

The whole engine is exposed through a single `ODDEngine` class. Load the
ontology and the ODD definition, push the current vehicle state in, then call
`inference()` to get the list of active restriction targets:

```cpp
#include <ODDEngine.h>
#include <iostream>

int main() {
  ODDEngine engine;
  engine.parse_ontology("ontology/ontology_fsw.rdf");
  engine.parse_odd("config/odd_fsw.yaml");

  // IRIs of ontology individuals are prefixed with the ODD's DNAMESPACE
  const std::string ns = engine.get_default_namespace();

  // Feed the current situation into the engine
  engine.set_data_property("egoVehicle.speed", 0.1);
  engine.set_sub_value("nextSection", ns + "tunnel");

  // Evaluate the ODD; returns e.g. {"MRM"} or {"ToR"}
  for (const std::string &target : engine.inference()) {
    std::cout << "active restriction: " << target << std::endl;
  }
  return 0;
}
```

Inside this repository, add a target to `CMakeLists.txt`:

```cmake
add_executable(my_app src/my_app.cpp $<TARGET_OBJECTS:cparse>)
target_link_libraries(my_app ODDEngine cparse ${REDLAND_LIBRARIES})
```

> **Note:** the `$<TARGET_OBJECTS:cparse>` part is required. CParse registers
> its operators from static initialisers that no symbol references, so if those
> objects are only reachable through a static archive the linker discards them
> and expressions fail at runtime with `Invalid operator: -`.

## Python example

The same `ODDEngine` class is exposed to Python one-to-one:

```python
import oddengine

engine = oddengine.ODDEngine()
engine.parse_ontology("ontology/ontology_fsw.rdf")
engine.parse_odd("config/odd_fsw.yaml")

# IRIs of ontology individuals are prefixed with the ODD's DNAMESPACE
ns = engine.get_default_namespace()

# Feed the current situation into the engine
engine.set_data_property("egoVehicle.speed", 0.1)
engine.set_sub_value("nextSection", ns + "tunnel")

# Evaluate the ODD; returns e.g. ["MRM"] or ["ToR"]
for target in engine.inference():
    print("active restriction:", target)
```

## Available methods

Identical in both languages:

| Method | Description |
| --- | --- |
| `parse_ontology(path)` | Load the RDF ontology exported from Protégé |
| `parse_odd(path)` | Load the ODD definition (YAML) |
| `set_data_property(key, value)` | Set a data property, e.g. `"egoVehicle.speed"`. Accepts `bool`, `int`, `float` |
| `set_sub_value(property, iri)` | Point an ontology object at a class individual by IRI |
| `inference()` | Evaluate the ODD; returns the list of active restriction targets |
| `get_default_namespace()` | The `DNAMESPACE` declared by the loaded ODD file |

Paths are resolved relative to the current working directory, so run the
examples from the repository root for the paths above to work.

## License

[MIT](https://github.com/MASKOR/ODD_Engine/blob/main/LICENSE)

## Author

 **Moritz Rumpf** & **Till Voss**
FH Aachen, Faculty of Electrical Engineering and Information Technology  
Supervisors: Prof. Dr.-Ing. Michael Reke, Joschua Schulte-Tigges M. Eng.

## Citation
If you use this project or its components in your research or in developing software, we kindly request that you cite it. Here's a suggested citation format:

```bibtex
@misc{ODDEngine,
  author = {Joschua Schulte-Tigges & Moritz Rumpf & Till Voss},
  title = {ODD Engine},
  year = {2025},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/MASKOR/ODD_Engine}}
}
```
