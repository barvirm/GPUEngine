#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <geGL/DebugMessage.h>
#include <geSG/Array.h>
#include <geSG/AttribsReference.h>
#include <geUtil/WindowObject.h>
#include <geUtil/ArgumentObject.h>

using namespace std;
using namespace ge::sg;


void Init();
void Idle();
void Mouse();
void Wheel(int d);


struct SContextParam{
  unsigned    Version;
  std::string Profile;
  std::string Flag;
}ContextParam;


struct SWindowParam{
  unsigned Size[2];
  bool     Fullscreen;
}WindowParam;


bool DisableAnttweakbar=true;
ge::util::ArgumentObject *Args;
ge::util::WindowObject   *Window;
//ge::gl::ProgramObject;


int main(int Argc,char*Argv[])
{
  Args=new ge::util::ArgumentObject(Argc,Argv);

  DisableAnttweakbar=true;

  //window args
  WindowParam.Size[0]    = atoi(Args->getArg("-w","800").c_str());
  WindowParam.Size[1]    = atoi(Args->getArg("-h","600").c_str());
  WindowParam.Fullscreen = Args->isPresent("-f");

  //context args
  ContextParam.Version = atoi(Args->getArg("--context-version","430").c_str());
  ContextParam.Profile = Args->getArg("--context-profile","core");
  ContextParam.Flag    = Args->getArg("--context-flag","debug");

  Window=new ge::util::WindowObject(
      WindowParam.Size[0],
      WindowParam.Size[1],
      WindowParam.Fullscreen,
      Idle,
      Mouse,
      !DisableAnttweakbar,
      ContextParam.Version,
      ContextParam.Profile,
      ContextParam.Flag);

  glewExperimental=GL_TRUE;
//  ge::gl::init
  glewInit();

  ge::gl::setDefaultDebugMessage();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);

  Init();
  Window->mainLoop();
  delete Window;
  delete Args;
  return 0;
}


void Mouse(){
}


void Wheel(int d){
}

static AttribsReference attribsRef;

void Idle(){
  Window->swap();
}


void Init(){
   AttribsConfig ac;
   ac.push_back(AttribType::Vec3);
   ac.ebo=false;
   ac.updateConfigId();
   attribsRef.allocData(ac,6,0);
#if 1
   shared_ptr<vector<glm::vec3>> data = make_shared<vector<glm::vec3>>();
   constexpr float z=-5.f;
   data->push_back(glm::vec3(0,0,z));
   data->push_back(glm::vec3(0,1,z));
   data->push_back(glm::vec3(1,0,z));
   data->push_back(glm::vec3(0,0,z));
   data->push_back(glm::vec3(0,1,z));
   data->push_back(glm::vec3(-1,0,z));
#endif
   vector<Array> v;
   v.resize(1);
   v[0].set(data);
   attribsRef.uploadVertexData(v);
}
