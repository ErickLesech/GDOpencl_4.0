#include "OCL.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

#include<fstream>

void OCL::_bind_methods() {
    //ClassDB::bind_method(D_METHOD("nom_methode"), &OCL::nom_methode);
   ClassDB::bind_method(D_METHOD("set","src"), &OCL::set);

   ClassDB::bind_method(D_METHOD("write","arr"), &OCL::write);
   
   ClassDB::bind_method(D_METHOD("run"), &OCL::run);
   ClassDB::bind_method(D_METHOD("read","arr"), &OCL::read);
 
   ClassDB::bind_method(D_METHOD("set_errMsg","msg"), &OCL::set_errMsg);
   ClassDB::bind_method(D_METHOD("get_errMsg"), &OCL::get_errMsg);
 //ADD_PROPERTY(PropertyInfo(string, "errMsg"),"set_errMsg","get_errMsg");

   ClassDB::bind_method(D_METHOD("set_log","msg"), &OCL::set_log);
   ClassDB::bind_method(D_METHOD("get_log"), &OCL::get_log);
 //ADD_PROPERTY(PropertyInfo(string, "log"),"set_log","get_log");
        
}


void OCL::set_errMsg(String str){

    this->errMsg = str;
}
        
String OCL::get_errMsg(){
    return this->errMsg;
}

void OCL::set_log(String str){
    this->log = str;
}

String OCL::get_log(){
    return this->log;
}


const char* getErrorString(cl_int err) {

    switch (err) {
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT _AVAILABLE";
    case -4: return "CL_MEM_OBJECT _ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT _AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT _MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT _SUPPORTED";

    default: return "ERROR";

    }
}

OCL::OCL() {
}

OCL::~OCL() {
    // add your cleanup here
}

void OCL::_init() {

    this->errMsg = "";
  
}


void OCL::set(String src) {

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    //const std::string SourcePath = "FindMind.cl";

    const char * source = src.utf8().get_data();

    if (platforms.empty()) {
        this->errMsg = "error not platform found";
        return;
    }

    // Get GPU device

    std::vector<cl::Device> devices;

    for (auto p = platforms.begin(); devices.empty() && p != platforms.end(); p++) {
        std::vector<cl::Device> pldev;

        try {
            p->getDevices(CL_DEVICE_TYPE_GPU, &pldev);

            for (auto d = pldev.begin(); devices.empty() && d != pldev.end(); d++) {

                if (!d->getInfo<CL_DEVICE_AVAILABLE>()) continue;

                std::string ext = d->getInfo<CL_DEVICE_EXTENSIONS>();

                devices.push_back(*d);

                this->context = cl::Context(devices);
                break;
            }
        }
        catch (...) {
            devices.clear();
        }

    }

    if (devices.empty()) {
        this->errMsg = "Devices not found";
        return;
    }

    //create a command queue

    this->queue = cl::CommandQueue(context, devices[0]);

    // Ligne de code si le code kernel est stocker dans un fichier

    //std::ifstream file(SourcePath);
    //std::string src = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
   
    // Compile opencl program for found device

    cl::Program program(context, cl::Program::Sources(1, std::make_pair(source, strlen(source))));

    cl_int err = program.build(devices, "-cl-std=CL1.2");


    if (err != CL_SUCCESS) {
        this->errMsg = "Failed OpeCL compilation error " + String(program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]).c_str());
        return;
    }

    this->kernel = cl::Kernel(program, "func");

    this->maxWorkGroupSize = this->kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[0], &err);


    if (err != CL_SUCCESS) {
        this->errMsg = "Failed getting opencl markgroupSize " + String(getErrorString(err));
        return;
    }

    this->outputBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * N);

}

void OCL::write(Array arr) {

    cl_int err;

    if (arr.size() != N) {
        char buff[16] = { 0 };

        sprintf(buff, "%d", N);
        this->errMsg = "Data array size not match : data size is " + String(buff);
    }

    this->data = std::vector<int>(N);

    for (int i = 0; i < N; i++) {
        int member = arr[i];
        data[i] = member;
    }

    this->inputBuffer = cl::Buffer(this->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(int), &data[0]);

    err = kernel.setArg(0, static_cast<cl_ulong>N);

    err |= kernel.setArg(1, inputBuffer);
    err |= kernel.setArg(2, outputBuffer);

    if (err != CL_SUCCESS) {
        this->errMsg = "Failed to set kernel args on writing : " + String(getErrorString(err));
        return;
    }



}

void OCL::run() {

    cl_int err;

    err = queue.enqueueNDRangeKernel(kernel, 0, N, cl::NDRange(1));

    if (err != CL_SUCCESS) {
        this->errMsg = "Failed run : " + String(getErrorString(err));
        return;
    }

    err = queue.finish();

    if (err != CL_SUCCESS) {
        this->errMsg = "Failed finish : " + String(getErrorString(err));
        return;
    }


}

void OCL::read(Array arr) {
    std::vector<int> res(N);

    cl_int err = queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, N * sizeof(int), &res[0]);

    if (err != CL_SUCCESS) {
        this->errMsg = "Failed err : " + String(getErrorString(err));
        return;
    }

    int success = 0;

    for (int i = 0; i < N; i++) {
        if (res[i] = data[i] * data[i]) {
            success++;
        }
    }

    char buff[32] = { 0 };
    sprintf(buff, "%d / %d", success, N);
    this->log = "Success " + String(buff);
}





