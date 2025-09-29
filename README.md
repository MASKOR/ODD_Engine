
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

## Dependencies
```bash
sudo apt install librdf0 librdf0-dev
```

CParse
```bash
cd odd_engine/include
git clone https://github.com/cparse/cparse.git
make release -C cparse
```

## Install
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

## Usage

- Define the ODD in a YAML file
- Create an ontology using [Protégé](https://github.com/protegeproject/protege) and export it as an RDF (.rdf) file
- Start the ODD engine
- The engine outputs active restriction keys that the ADS can use to adjust driving behavior

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
