#include "ODDEngine.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(oddengine, m) {
  m.doc() = "Python bindings for the C++ ODD Engine library";

  py::class_<ODDEngine>(m, "ODDEngine")
      .def(py::init<>())
      .def("parse_ontology", &ODDEngine::parse_ontology, py::arg("path"),
           "Load the RDF ontology from disk.")
      .def("parse_odd", &ODDEngine::parse_odd, py::arg("path"),
           "Load the ODD definition (YAML) from disk.")

      .def("set_data_property", &ODDEngine::set_data_property<bool>,
           py::arg("Set a value of a data property"), py::arg("value"))
      .def("set_data_property", &ODDEngine::set_data_property<double>,
           py::arg("Set a value of a data property"), py::arg("value"))
      .def("set_data_property", &ODDEngine::set_data_property<int>,
           py::arg("Set a value of a data property"), py::arg("value"))
      .def("set_data_property", &ODDEngine::set_data_property<float>,
           py::arg("Set a value of a data property"), py::arg("value"))

      .def("set_sub_value", &ODDEngine::set_sub_value, py::arg("property"),
           py::arg("Set a sub value (IRI) of a ontology class object"))

      .def("inference", &ODDEngine::inference,
           "Evaluate the ODD against the current state and return the list of "
           "active restriction targets.")

      .def("get_default_namespace", &ODDEngine::get_default_namespace,
           "The DNAMESPACE declared by the loaded ODD file.");
}
