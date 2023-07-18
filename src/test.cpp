#include <tcl.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <stdlib.h>

public class TclInterpreter {
private:
    Tcl_Interp *interp;
public:
    TclInterpreter(const char *argv0 = nullptr) : interp(nullptr) {
        static bool initLib;
        if (!initLib) {
            Tcl_FindExecutable(argv0);
            initLib = true;
        }
        interp = Tcl_CreateInterp();
        if (!interp) throw new std::runtime_error("failed to initialise Tcl library");
    }

    ~TclInterpreter() {
        if (interp) Tcl_DeleteInterp(interp);
    }

    std::string evalFile(const std::string &filename) {
        // This next line is the CRITICAL one!
        int code = Tcl_EvalFile(interp, filename.c_str());

        if (code >= TCL_ERROR) {
            // You should make your own exception, but I've lost patience typing it out
            // This throws the exception message; it lasts until you use the interpreter again
            throw Tcl_GetStringResult(interp);
        }
        return std::string(Tcl_GetStringResult(interp));
    }
};

int main(int argc, char **argv)
{
    // TclInterpreter interpreter(argv[0]); // << omit ‘argv[0]’ if you don't know it
    putenv("LMTDIR=\"c:/Program Files/IST 8.1.1 - omp i17/lmt-8.1.1/\"");
    putenv("LMTSYS=wxx_n64_omp_i17");
    TclInterpreter interpreter(0); // << omit ‘argv[0]’ if you don't know it
    // std::string filename("C:/projects/gui/src/test.tcl");
    std::string filename("c:/Program Files/IST 8.1.1 - omp i17/lmt-8.1.1/tools/common/zwi2tab.tcl");
    std::string result = interpreter.evalFile(filename);
    std::cout << result << '\n';
    std::cout << "Test" << std::endl;

    return 0;
}
