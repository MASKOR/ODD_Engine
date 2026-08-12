
# ODD Engine

This repository implements an ODD-oriented definition language and a runtime environment for monitoring and restricting the functionality of an Automated Driving System (ADS) based on predefined Operational Design Domains (ODD).

## Contents

- **Ontology:** Knowledge structure based on OWL, defining attributes and relations within the ODD.
- **Database:** Storage and access to instances (objects) of the ontology and evaluation results.
- **Expressions:** Mathematical and logical expressions for calculating and verifying ODD conditions.
- **ODD Engine:** Evaluation of the current vehicle situation based on defined rules and restrictions.
- **Integration:** ROS2 node to connect the engine with an existing ADS (e.g., from the ADP project at FH Aachen).

## Features

- Definition of ODDs in a human-readable YAML format
- Support for:
  - Mathematical and logical expressions
  - Combination of rules (Guardrails)
  - Multi-level ontologies for complex environmental conditions
- Output of restriction keys (e.g., ToR or MRM) to control ADS behavior
- Modern C++17 implementation using smart pointers and standard library features

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
which drives the same CMake build. From the repository root:

```bash
pip install .
```

This compiles the engine and installs an `oddengine` extension module into your
environment, so `import oddengine` works from any directory. A virtual
environment is recommended; without one, use `pip install --user .`.

The install is a snapshot — re-run `pip install .` after changing any C++
source. Use `pip install -e .` instead if you want an editable install that
rebuilds on demand.

The module is also produced as `build/oddengine.cpython-*.so` by a plain CMake
build, but it is only importable from that directory unless you install it or
put `build/` on `PYTHONPATH`.



### Python example

The whole engine is exposed through a single `ODDEngine` class. Load the
ontology and the ODD definition, push the current vehicle state in, then call
`inference()` to get the list of active restriction targets:

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

Available methods:

| Method | Description |
| --- | --- |
| `parse_ontology(path)` | Load the RDF ontology exported from Protégé |
| `parse_odd(path)` | Load the ODD definition (YAML) |
| `set_data_property(key, value)` | Set a data property, e.g. `"egoVehicle.speed"`. Accepts `bool`, `int`, `float` |
| `set_sub_value(property, iri)` | Point an ontology object at a class individual by IRI |
| `inference()` | Evaluate the ODD; returns the list of active restriction targets |
| `get_default_namespace()` | The `DNAMESPACE` declared by the loaded ODD file |

Paths are resolved relative to the current working directory, so run the script
from the repository root for the paths above to work.

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
