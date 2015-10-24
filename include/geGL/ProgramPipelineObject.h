#pragma once

#include<geGL/OpenGL.h>
#include<geGL/OpenGLObject.h>
namespace ge{
  namespace gl{
    class GEGL_EXPORT ProgramPipelineObject: public OpenGLObject{
      public:
        ProgramPipelineObject();
        ~ProgramPipelineObject();
        void bind()const;
        void unbind()const;
        void useProgramStages(GLbitfield stages,GLuint program)const;
        void useVertexStage    (GLuint program)const;
        void useControlStage   (GLuint program)const;
        void useEvaluationStage(GLuint program)const;
        void useGeometryStage  (GLuint program)const;
        void useFragmentStage  (GLuint program)const;
        void useComputeStage   (GLuint program)const;
    };
  }//gl
}//ge

