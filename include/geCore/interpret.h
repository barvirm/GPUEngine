#pragma once

#include<geCore/Command.h>
#include<geCore/TypeRegister.h>

namespace ge{
  namespace core{
    class GECORE_EXPORT Statement: public ge::core::Command{
      public:
        enum Type{
          FUNCTION,
          BODY    ,
          WHILE   ,
          IF      ,
        };
      protected:
        Type _type;
      public:
        Statement(Type type);
        virtual ~Statement();
        virtual void operator()()=0;
    };

    class GECORE_EXPORT Function;
    class GECORE_EXPORT FunctionInput{
      public:
        std::shared_ptr<Function>      function    = nullptr                   ;
        bool                           changed     = false                     ;
        ge::core::TypeRegister::TypeID type        = TypeRegister::UNREGISTERED;
        unsigned long long             updateTicks = 0                         ;
        FunctionInput(
            std::shared_ptr<Function>      fce         = nullptr                             ,
            unsigned long long             updateTicks = 0                                   ,
            bool                           changed     = false                               ,
            ge::core::TypeRegister::TypeID type        = ge::core::TypeRegister::UNREGISTERED);
    };

    class MacroFunction;
    class GECORE_EXPORT Function: public Statement{
      protected:
        friend class MacroFunction;
        std::string                        _name      ;
        std::vector<FunctionInput>         _inputs    ;
        std::map<unsigned,std::string>     _input2Name;
        std::map<std::string,unsigned>     _name2Input;
        struct Output{
          std::shared_ptr<ge::core::Accessor>data = nullptr                   ;
          ge::core::TypeRegister::TypeID     type = TypeRegister::UNREGISTERED;
          std::string                        name = "output"                  ;
        }_output;
        unsigned long long                 _checkTicks  = 0;
        unsigned long long                 _updateTicks = 1;
        std::string _genDefaultName(unsigned i)const;
        void _defaultNames(unsigned n);
        void _processInputs();
        void _setOutput(           ge::core::TypeRegister::TypeID type,std::string name = "");
        void _setInput (unsigned i,ge::core::TypeRegister::TypeID type,std::string name = "");
        virtual bool _do();
        inline bool _inputChanged(unsigned    i    )const;
        inline bool _inputChanged(std::string input)const;
        virtual inline FunctionInput      &_getInput(unsigned i);
        virtual inline FunctionInput const&_getInput(unsigned i)const;
        virtual inline decltype(_inputs)::size_type _getNofInputs()const;
        virtual inline Output      &_getOutput();
        virtual inline Output const&_getOutput()const;
        template<typename... ARGS>
          unsigned _computeNumberOfInputs(ARGS...args){
            return (sizeof...(args)-2)/2;
          }
        template<typename... ARGS>
          void _setTypes(
              std::shared_ptr<ge::core::TypeRegister>const&tr,
              unsigned nofInputs,
              const char* type,
              const char* name,
              ARGS... args){
            this->_setOutput(tr->getTypeId(type),name);
            this->_setInputs(tr,nofInputs,args...);
          }
        template<typename... ARGS>
          void _setInputs(
              std::shared_ptr<ge::core::TypeRegister>const&tr,
              unsigned nofInputs,
              const char* type,
              const char* name,
              ARGS... args){
            this->_setInput(nofInputs-(sizeof...(args))/2-1,tr->getTypeId(type),name);
            this->_setInputs(tr,nofInputs,args...);
          }
        template<typename... ARGS>
          void _setInputs(
              std::shared_ptr<ge::core::TypeRegister>const&,
              unsigned){
          }
      public:
        Function(unsigned n,std::string name = "");
        virtual ~Function();
        virtual void operator()();
        inline std::shared_ptr<ge::core::Accessor>const&getOutput()const;
        inline ge::core::TypeRegister::TypeID getOutputType()const;
        inline std::string getOutputName()const;
        inline bool hasInput(unsigned    i   )const;
        inline bool hasInput(std::string name)const;
        inline bool hasOutput()const;
        bool bindInput (unsigned    i   ,std::shared_ptr<Function>function=nullptr);
        bool bindInput (std::string name,std::shared_ptr<Function>function=nullptr);
        bool bindOutput(std::shared_ptr<ge::core::Accessor>data = nullptr);
        inline std::shared_ptr<ge::core::Accessor>const&getInputData(unsigned i       )const;
        inline std::shared_ptr<ge::core::Accessor>const&getInputData(std::string input)const;
        inline ge::core::TypeRegister::TypeID getInputType(unsigned    i   )const;
        inline ge::core::TypeRegister::TypeID getInputType(std::string name)const;
        inline decltype(_inputs)::size_type getNofInputs()const;
        virtual inline std::string getInputName (unsigned    i   )const;
        virtual inline unsigned    getInputIndex(std::string name)const;
        virtual inline std::string doc()const;
    };

    inline std::shared_ptr<ge::core::Accessor>const&Function::getOutput()const{
      return this->_getOutput().data;
    }

    inline ge::core::TypeRegister::TypeID Function::getOutputType()const{
      return this->_getOutput().type;
    }

    inline std::string Function::getOutputName()const{
      return this->_getOutput().name;
    }

    inline bool Function::hasInput(unsigned i)const{
      return this->_getInput(i).function!=nullptr;
    }

    inline bool Function::hasInput(std::string name)const{
      return this->hasInput(this->getInputIndex(name));
    }

    inline std::string Function::doc()const{
      return"";
    }

    inline bool Function::hasOutput()const{
      return this->_getOutput().data!=nullptr;
    }

    inline std::shared_ptr<ge::core::Accessor>const&Function::getInputData(unsigned i       )const{
      return this->_getInput(i).function->getOutput();
    }

    inline std::shared_ptr<ge::core::Accessor>const&Function::getInputData(std::string input)const{
      return this->getInputData(this->_name2Input.find(input)->second);
    }

    inline ge::core::TypeRegister::TypeID Function::getInputType(unsigned    i   )const{
      return this->_getInput(i).type;
    }

    inline ge::core::TypeRegister::TypeID Function::getInputType(std::string name)const{
      return this->_getInput(this->getInputIndex(name)).type;
    }

    inline decltype(Function::_inputs)::size_type Function::getNofInputs()const{
      return this->_inputs.size();
    }

    inline bool Function::_inputChanged(unsigned i)const{
      return this->_getInput(i).changed;
    }

    inline bool Function::_inputChanged(std::string input)const{
      return this->_inputChanged(this->_name2Input.find(input)->second);
    }

    inline FunctionInput&Function::_getInput(unsigned i){
      return this->_inputs[i];
    }

    inline FunctionInput const&Function::_getInput(unsigned i)const{
      return this->_inputs[i];
    }

    inline decltype(Function::_inputs)::size_type Function::_getNofInputs()const{
      return this->_inputs.size();
    }

    inline Function::Output      &Function::_getOutput(){
      return this->_output;
    }

    inline Function::Output const&Function::_getOutput()const{
      return this->_output;
    }

    inline std::string Function::getInputName(unsigned i)const{
      if(i>=this->_getNofInputs()){
        std::cerr<<"ERROR: "<<this->_name<<"::getInputName("<<i<<") - ";
        std::cerr<<"input out of range"<<std::endl;
        return "";
      }
      return this->_input2Name.find(i)->second;
    }

    inline unsigned Function::getInputIndex(std::string name)const{
      auto ii=this->_name2Input.find(name);
      if(ii==this->_name2Input.end()){
        std::cerr<<"ERROR: "<<this->_name<<"::getInputIndex(\""<<name<<"\") - ";
        std::cerr<<"there is no input with that name"<<std::endl;
        return -1;
      }
      return ii->second;
    }

    class GECORE_EXPORT Body: public Statement{
      protected:
        std::vector<std::shared_ptr<Statement>>_statements;
      public:
        Body();
        virtual ~Body();
        void addStatement(std::shared_ptr<Statement>const&statement);
        std::vector<std::shared_ptr<Statement>>::size_type size()const;
        typedef std::vector<std::shared_ptr<Statement>>::const_iterator Iterator;
        Iterator begin()const;
        Iterator end  ()const;
        virtual void operator()();
    };

    class GECORE_EXPORT While: public Statement{
      protected:
        std::shared_ptr<Function >_condition = nullptr;
        std::shared_ptr<Statement>_body      = nullptr;
      public:
        While(std::shared_ptr<Function>const&condition = nullptr,std::shared_ptr<Statement>const&body = nullptr);
        virtual ~While();
        void setBody     (std::shared_ptr<Statement>const&body      = nullptr);
        void setCondition(std::shared_ptr<Function >const&condition = nullptr);
        std::shared_ptr<Function >const&getCondition()const;
        std::shared_ptr<Statement>const&getBody     ()const;
        virtual void operator()();
    };

    class GECORE_EXPORT If: public Statement{
      protected:
        std::shared_ptr<Function >_condition = nullptr;
        std::shared_ptr<Statement>_trueBody  = nullptr;
        std::shared_ptr<Statement>_falseBody = nullptr;
      public:
        If(
            std::shared_ptr<Function >const&condition = nullptr,
            std::shared_ptr<Statement>const&trueBody  = nullptr,
            std::shared_ptr<Statement>const&falseBody = nullptr);
        virtual ~If();
        void setCondition(std::shared_ptr<Function >const&condition = nullptr);
        void setTrueBody (std::shared_ptr<Statement>const&trueBody  = nullptr);
        void setFalseBody(std::shared_ptr<Statement>const&falseBody = nullptr);
        std::shared_ptr<Function >const&getCondition()const;
        std::shared_ptr<Statement>const&getTrueBody ()const;
        std::shared_ptr<Statement>const&getFalseBody()const;
        virtual void operator()();
    };
  }
}

#define BEGIN_INTERPRET_FUNCTION_HPP(NAME)\
  class GECORE_EXPORT NAME: public ge::core::Function{\
    protected:\
              bool _do();\
    public:\
           static inline std::string name(){return #NAME;}\
    static inline std::shared_ptr<ge::core::Function> sharedInstance(std::shared_ptr<ge::core::TypeRegister>const&tr){\
      return std::make_shared<NAME>(tr);\
    }\
    NAME(std::shared_ptr<ge::core::TypeRegister>const&typeRegister)
#define END_INTERPRET_FUNCTION_HPP()\
  }

#define NEW_INTERPRET_FUNCTION_HPP(NAME)\
  BEGIN_INTERPRET_FUNCTION_HPP(NAME);\
END_INTERPRET_FUNCTION_HPP()

#define BEGIN_INTERPRET_FUNCTION_CPP(NAME,...)\
  NAME::NAME(std::shared_ptr<ge::core::TypeRegister>const&tr):Function(this->_computeNumberOfInputs(__VA_ARGS__),NAME::name()){\
    this->_setTypes(tr,this->_computeNumberOfInputs(__VA_ARGS__),__VA_ARGS__)

#define MID_INTERPRET_FUNCTION_CPP(NAME)\
  }\
bool NAME::_do(){

#define END_INTERPRET_FUNCTION_CPP()\
}

#define NEW_INTERPRET_FUNCTION_CPP(NAME,...)\
  BEGIN_INTERPRET_FUNCTION_CPP(NAME,__VA_ARGS__)\
BODY_OF_INTERPRET_FUNCTION\
END_INTERPRET_FUNCTION_CPP()

