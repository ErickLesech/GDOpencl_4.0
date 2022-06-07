#ifndef OCL_H
#define OCL_H

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/object.hpp>

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/viewport.hpp>

#include <godot_cpp/core/binder_common.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <godot_cpp/CL/cl.hpp>

using std::vector;
using std::cout;
using std::endl;

#define _CL_ENABLE_EXCEPTIONS

#define N ( 1 << 10 )
#define MAX_RANDOM ( 1 << 10)


namespace godot {

    class OCL : public Object {
        GDCLASS(OCL, Object);

    private:

        size_t maxWorkGroupSize;

        cl::Kernel kernel;
        cl::CommandQueue queue;
        cl::Context context;

        cl::Buffer inputBuffer, outputBuffer;

        std::vector<cl::Device> devices;

        std::vector<int> data;

        String errMsg;
        String log;

    protected:
        static void _bind_methods();

    public:
        OCL();
        ~OCL();

        
        void set(String src);
        void write(Array arr);
        void run();
        void read(Array arr);

        //getter et setter

        void set_errMsg(String str);
        String get_errMsg();

        void set_log(String str);
        String get_log();


        
        void _init(); // our initializer called by Godot

    };

};

#endif