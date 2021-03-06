/**
   @author Shin'ichiro Nakaoka
*/

#include "OptionManager.h"
#include <iostream>
#include <set>

using namespace std;
using namespace boost;
using namespace cnoid;


namespace {

    struct OptionInfo {
        OptionInfo() : options("Options") { }
        program_options::options_description options;
        program_options::positional_options_description positionalOptions;
        program_options::variables_map variables;
    };

    OptionInfo* info = 0;
}


bool OptionManager::parseCommandLine(int argc, char *argv[])
{
    if(!info){
        info = new OptionInfo;
    }
    
    info->options.add_options()("help,h", "show help message");

    bool is_error = false;
    try{
        program_options::store(
            program_options::command_line_parser(argc, argv).
            options(info->options).positional(info->positionalOptions).run(), info->variables);
    
        program_options::notify(info->variables);
    } catch (std::exception& ex) {
        std::cerr << "Command line option error! : " << ex.what() << std::endl;
        is_error = true;
    }

    bool terminated;
    
    if(info->variables.count("help") || is_error){
        cout << info->options << endl;
        terminated = true;
    } else {
        sigOptionsParsed_(info->variables);
        terminated = false;
    }

    // The destructors of the OptionInfo members should be executed here
    // because their elements may be driven by the code instantiated in the plug-in dlls.
    // If the destructors are called when the program is finished after the plug-ins are
    // unloaded, the destructors may cause the segmentation fault
    // by calling the non-existent codes.
    delete info;
    info = 0;
    sigOptionsParsed_.disconnect_all_slots();

    return !terminated;
}


OptionManager::OptionManager()
{
    info = new OptionInfo;
}


OptionManager::~OptionManager()
{

}


/*
boost::program_options::options_description_easy_init OptionManager::addOptions()
{
    return options.add_options();
}
*/


void OptionManager::addOption(const char* name, const char* description)
{
    info->options.add_options()(name, description);
}


void OptionManager::addOption(const char* name, const program_options::value_semantic* s)
{
    info->options.add_options()(name, s);
}


void OptionManager::addOption(const char* name, const program_options::value_semantic* s, const char* description)
{
    info->options.add_options()(name, s, description);
}


void OptionManager::addPositionalOption(const char* name, int maxCount)
{
    info->positionalOptions.add(name, maxCount);
}
