/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object* obj);
void freeScope(Scope* scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab* symtab;
Type* intType;
Type* charType;

/******************* Type utilities ******************************/
// tao kieu int
Type* makeIntType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

//tao kieu char
Type* makeCharType(void) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

// tao mang
Type* makeArrayType(int arraySize, Type* elementType) {
  Type* type = (Type*) malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

//loai trung lap
Type* duplicateType(Type* type) {
  // TODO
  Type * newType = (Type*) malloc(sizeof(Type));
  newType->typeClass = type->typeClass;
  newType->arraySize = type->arraySize;
  newType->elementType = type->elementType;

  return newType;
}

//so sanh 2 kieu voi nhau
int compareType(Type* type1, Type* type2) {
  // TODO
  if( type1->typeClass == type2->typeClass ) {
    if( type1->typeClass == TP_ARRAY ) {
      if(!(type1->arraySize == type2->arraySize && compareType(type1->elementType, type2->elementType)))
        return 0;
    }
    return 1;
  }
  return 0;
}

//giai phong type
void freeType(Type* type) {
  // TODO
  if(type->elementType != NULL){
    freeType(type->elementType);
  }

  free(type);
  type = NULL;
}

/******************* Constant utility ******************************/

//tao gia tri hang so int
ConstantValue* makeIntConstant(int i) {
  // TODO
  // value: constValue
  ConstantValue * value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_INT;
  value->intValue = i;

  return value;
}

// tao gia tri hang so char
ConstantValue* makeCharConstant(char ch) {
  // TODO
  ConstantValue * value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_CHAR;
  value->charValue = ch;

  return value;
}

// tao gia tri hang so duplicate
ConstantValue* duplicateConstantValue(ConstantValue* v) {
  // TODO
  ConstantValue * value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = v->type;

  if(v->type == TP_CHAR){
    value->charValue = v->charValue;
  }else if(v->type == TP_INT){
    value->intValue = v->intValue;
  }

  return value;
}

/******************* Object utilities ******************************/

// tao scope ms
Scope* CreateScope(Object* owner, Scope* outer) {
  Scope* scope = (Scope*) malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

// tao chuong trinh
Object* CreateProgramObject(char *programName) {
  Object* program = (Object*) malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes*) malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = CreateScope(program,NULL);
  symtab->program = program;

  return program;
}

// tao doi tuong hang so
Object* CreateConstantObject(char *name) {
  // TODO
  Object* constObj = (Object *) malloc(sizeof(Object));
  strcpy(constObj->name, name);
  constObj->kind = OBJ_CONSTANT;
  constObj->constAttrs = (ConstantAttributes *) malloc(sizeof(ConstantAttributes));
  constObj->constAttrs->value = NULL;

  return constObj;
}

Object* CreateTypeObject(char *name) {
  // TODO
  Object* type = (Object*) malloc(sizeof(Object));
  strcpy(type->name, name);
  type->typeAttrs = (TypeAttributes*) malloc(sizeof(TypeAttributes));
  type->typeAttrs->actualType = NULL;
  type->kind = OBJ_TYPE;

  return type;
}

Object* CreateVariableObject(char *name) {
  // TODO
  Object* var = (Object*) malloc(sizeof(Object));
  strcpy(var->name, name);
  var->varAttrs = (VariableAttributes*) malloc(sizeof(VariableAttributes));
  var->kind = OBJ_VARIABLE;

  var->varAttrs->type = NULL;
  var->varAttrs->scope = symtab->currentScope;

  return var;
}

Object* CreateFunctionObject(char *name) {
  // TODO
  Object* func = (Object*) malloc(sizeof(Object));
  strcpy(func->name, name);
  func->kind = OBJ_FUNCTION;

  func->funcAttrs = (FunctionAttributes*) malloc(sizeof(FunctionAttributes));
  func->funcAttrs->returnType = NULL;
  func->funcAttrs->paramList = NULL;
  func->funcAttrs->scope = CreateScope(func, symtab->currentScope);

  return func;
}

Object* CreateProcedureObject(char *name) {
  // TODO
  Object* procedure = (Object*) malloc(sizeof(Object));
  strcpy(procedure->name, name);
  procedure->kind = OBJ_PROCEDURE;

  procedure->procAttrs = (ProcedureAttributes*) malloc(sizeof(ProcedureAttributes));
  procedure->procAttrs->paramList = NULL;
  procedure->procAttrs->scope = CreateScope(procedure, symtab->currentScope);

  return procedure;
}

Object* CreateParameterObject(char *name, enum ParamKind kind, Object* owner) {
  // TODO
  Object* param = (Object*) malloc(sizeof(Object));
  strcpy(param->name, name);
  param->kind = OBJ_PARAMETER;

  param->paramAttrs = (ParameterAttributes*) malloc(sizeof(ParameterAttributes));
  param->paramAttrs->kind = kind;
  param->paramAttrs->function = owner;
  param->paramAttrs->type = NULL;

  return param;
}

void freeObject(Object* obj) {
  // TODO
      if (obj != NULL) {
        if (obj->constAttrs != NULL) {
          switch (obj->kind) {
          
          case OBJ_VARIABLE:
            if (obj->varAttrs->type != NULL) {
                free(obj->varAttrs->type);
                obj->varAttrs->type = NULL;
            }
            break;
          case OBJ_CONSTANT:
            if (obj->constAttrs->value != NULL) {
                free(obj->constAttrs->value);
                obj->constAttrs->value = NULL;
            }
            break;
          
          case OBJ_PROGRAM:
            if (obj->progAttrs->scope != NULL) {
                freeScope(obj->progAttrs->scope);
                obj->progAttrs->scope = NULL;
            }
            break;
          case OBJ_TYPE:
            if (obj->typeAttrs->actualType != NULL) {
                free(obj->typeAttrs->actualType);
                obj->typeAttrs->actualType = NULL;
            }
            break;
          case OBJ_PARAMETER:
            if (obj->paramAttrs->type != NULL) {
              free(obj->paramAttrs->type);
              obj->paramAttrs->type = NULL;
            }
            break;
          case OBJ_PROCEDURE:
            freeScope(obj->procAttrs->scope); // Free scope also free the param list
            break;  
          case OBJ_FUNCTION:
            freeScope(obj->funcAttrs->scope); // Free scope also free the param list
            break;
          
          default:
              break;
          }
          free(obj->constAttrs);
          obj->constAttrs = NULL;
        }
        free(obj);
        obj = NULL;
    }
}

void freeScope(Scope* scope) {
  // TODO
  if (scope != NULL) {
    freeObjectList(scope->objList);
    free(scope);
    scope = NULL;
  }
}

void freeObjectList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
    freeObject(objList->object);
    freeObjectList(objList->next);
    objList = NULL;
  }
}

void freeReferenceList(ObjectNode *objList) {
  // TODO
  if (objList != NULL) {
    freeObject(objList->object);
    freeReferenceList(objList->next);
    objList = NULL;
  }
}

void AddObject(ObjectNode **objList, Object* obj) {
  ObjectNode* node = (ObjectNode*) malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL) 
    *objList = node;
  else {
    ObjectNode *n = *objList;
    while (n->next != NULL) 
      n = n->next;
    n->next = node;
  }
}

Object* findObject(ObjectNode *objList, char *name) {
  // TODO
  ObjectNode * currentNode = objList;
  while(currentNode != NULL) {
    if (strcmp(currentNode->object->name, name) == 0) {
      return currentNode->object;
    }
    currentNode = currentNode->next;
  }

  return NULL;
}

/******************* others ******************************/

void initSymTab(void) {
  Object* obj;
  Object* param;

  symtab = (SymTab*) malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;
  
  obj = CreateFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEI");
  param = CreateParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITEC");
  param = CreateParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  AddObject(&(obj->procAttrs->paramList),param);
  AddObject(&(symtab->globalObjectList), obj);

  obj = CreateProcedureObject("WRITELN");
  AddObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void) {
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

void exitBlock(void) {
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner = symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      AddObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      AddObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }
 
  AddObject(&(symtab->currentScope->objList), obj);
}


