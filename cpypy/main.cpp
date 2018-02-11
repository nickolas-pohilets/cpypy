#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/filesystem.hpp>
#include <Python/Python.h>
#include <iostream>

namespace py = boost::python;
namespace np = boost::python::numpy;
namespace fs = boost::filesystem;

class PyCode {
    std::string code_;
    char const * file_;
    int kind_;
public:
    static PyCode from_raw(int line, char const * file, int kind, char const * code) {
        // macro __LINE__ expands into a line where expression ends
        for (char const * s = code; *s; s++) {
            if (*s == '\n') {
                line -= 1;
            }
        }
        
        return PyCode(line, file, kind, code);
    }
    
    PyCode(int line, char const * file, int kind, char const * code)
        // There seems to be no Python API for running/compiling code with custom line number.
        // So instead we add neccessary amount of empty lines in the beginning of the code string.
        : code_(line - 1, '\n')
        , file_(file)
        , kind_(kind)
    {
        code_ += code;
    }
    
    py::object eval(py::object globals, py::object locals = py::object()) {
        PyObject* code = Py_CompileString(code_.c_str(), file_, kind_);
        if (locals.is_none()) {
            locals = py::dict();
        }
        PyObject * result = PyEval_EvalCode(code, globals.ptr(), locals.ptr());
        Py_DecRef(code);
        return py::object(py::handle<PyObject>(result));
    }
};

#define PY_CODE(X) PyCode::from_raw(__LINE__, __FILE__, Py_file_input, X)
#define PY_EXPR(X) PyCode(__LINE__, __FILE__, Py_eval_input, X)

auto pydevd_init = PY_CODE(R"(
import pydevd;
pydevd.settrace('localhost', port=5678, stdoutToServer=True, stderrToServer=True)
)");

auto inline_code = PY_CODE(R"(
import numpy as np
import scipy.misc
import os.path
scipy.misc.imsave(os.path.expanduser('~/foo.png'), data)
)");

int main(int argc, const char * argv[]) {
    Py_Initialize();
    np::initialize();
    {
        py::object main = py::import("__main__");
        py::object global(main.attr("__dict__"));
        
        // This line tries to connect to Python debug server, that should be started first.
        pydevd_init.eval(global);
        
        // The following line contains infinite loop for demonstration purposes.
        // It can be stopped by setting variable 'stop' to True from Python debugger.
        py::exec_file(fs::path(__FILE__).parent_path().append("fibonacci.py").c_str(), global);
        
        uint8_t data[][4] = {{1,2,3,255},{255,6,7,8},{1,128,5,7}};
        np::ndarray np_data = np::from_data(
            data,
            np::dtype::get_builtin<uint8_t>(),
            py::make_tuple(3,4),
            py::make_tuple(4,1),
            py::object()
        );
        py::dict locals;
        locals["data"] = np_data;
        
        inline_code.eval(global, locals);
        
        std::cout << py::extract<int>(PY_EXPR("5 ** 2").eval(global, py::object())) << std::endl;
    }
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    return 0;
}
