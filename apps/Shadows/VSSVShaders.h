#pragma once

#include<iostream>
std::string const _drawSidesVertexShaderSrc = R".(
#ifndef MAX_MULTIPLICITY
#define MAX_MULTIPLICITY 2
#endif//MAX_MULTIPLICITY

#ifndef USE_PLANES_INSTEAD_OF_OPPOSITE_VERTICES

layout(location=0)in vec3  edgeVertexA        ;
layout(location=1)in vec3  edgeVertexB        ;
layout(location=2)in float nofOppositeVertices;
layout(location=3)in vec3  oppositeVertices[MAX_MULTIPLICITY];

uniform vec4 lightPosition    = vec4(10,10,10,1);
uniform mat4 modelMatrix      = mat4(1.f);
uniform mat4 viewMatrix       = mat4(1.f);
uniform mat4 projectionMatrix = mat4(1.f);

int greaterVec(vec3 a,vec3 b){
  return int(dot(sign(a-b),vec3(4.0,2.0,1.0)));
}

int computeMult(vec3 A,vec3 B,vec3 C,vec4 L){
  vec3 n=cross(C-A,L.xyz-A*L.w);
  return int(sign(dot(n,B-A)));
}

int currentMultiplicity(vec3 A,vec3 B,vec3 O,vec4 L){
  return int(sign(dot(cross(B-A,L.xyz-A),O-A)));
  if(greaterVec(A,O)>0)
    return computeMult(O,A,B,L);
  else if(greaterVec(B,O)>0)
    return -computeMult(A,O,B,L);
  else
    return computeMult(A,B,O,L);
}

void main(){
  mat4 mvp = projectionMatrix*viewMatrix*modelMatrix;
#ifdef USE_TRIANGLE_STRIPS
  int vertexIDCCW = gl_VertexID;
  int vertexIDCW  = gl_VertexID^0x1;
#else
  int vertexIDCCW = int(gl_VertexID>2?gl_VertexID-2:2-gl_VertexID);
  int vertexIDCW  = int(gl_VertexID>2?6-gl_VertexID:gl_VertexID);
#endif//USE_TRIANGLE_STRIPS
  int sideID      = gl_InstanceID%MAX_MULTIPLICITY;
  vec4 P[4];
  P[0]=vec4(edgeVertexA.xyz,1);
  P[1]=vec4(edgeVertexB.xyz,1);
  P[2]=vec4(P[0].xyz*lightPosition.w-lightPosition.xyz,0);
  P[3]=vec4(P[1].xyz*lightPosition.w-lightPosition.xyz,0);
  int multiplicity = 0;
  for(uint m=0;m<uint(nofOppositeVertices);++m)
    multiplicity -= currentMultiplicity(P[0].xyz,P[1].xyz,oppositeVertices[m].xyz,lightPosition);
  if(sideID<int(abs(float(multiplicity)))){
    if(multiplicity>0)
      gl_Position=mvp*P[vertexIDCCW];
    if(multiplicity<0)
      gl_Position=mvp*P[vertexIDCW];
  }else
    gl_Position=vec4(0,0,0,0);
}

#else//USE_PLANES_INSTEAD_OF_OPPOSITE_VERTICES
layout(location=0)in vec3  edgeVertexA      ;
layout(location=1)in vec3  edgeVertexB      ;
#ifndef USE_ALL_OPPOSITE_VERTICES
layout(location=2)in float nofOppositeVertices;
#endif//USE_ALL_OPPOSITE_VERTICES
layout(location=3)in vec4  trianglePlanes[MAX_MULTIPLICITY];

uniform vec4 lightPosition    = vec4(10,10,10,1);
uniform mat4 modelMatrix      = mat4(1.f);
uniform mat4 viewMatrix       = mat4(1.f);
uniform mat4 projectionMatrix = mat4(1.f);

void main(){
  mat4 mvp = projectionMatrix*viewMatrix*modelMatrix;
#ifdef USE_TRIANGLE_STRIPS
  int vertexIDCCW = gl_VertexID;
  int vertexIDCW  = gl_VertexID^0x1;
#else
  int vertexIDCCW = int(gl_VertexID>2?gl_VertexID-2:2-gl_VertexID);
  int vertexIDCW  = int(gl_VertexID>2?6-gl_VertexID:gl_VertexID);
#endif//USE_TRIANGLE_STRIPS
  int sideID      = gl_InstanceID%MAX_MULTIPLICITY;
  vec4 P[4];
  P[0]=vec4(edgeVertexA.xyz,1);
  P[1]=vec4(edgeVertexB.xyz,1);
  P[2]=vec4(P[0].xyz*lightPosition.w-lightPosition.xyz,0);
  P[3]=vec4(P[1].xyz*lightPosition.w-lightPosition.xyz,0);
  int multiplicity = 0;
#ifdef USE_ALL_OPPOSITE_VERTICES
  for(uint m=0;m<MAX_MULTIPLICITY;++m)
#else
  for(uint m=0;m<uint(nofOppositeVertices);++m)
#endif//USE_ALL_OPPOSITE_VERTICES
    multiplicity += int(sign(dot(trianglePlanes[m],lightPosition)));
  if(sideID<int(abs(float(multiplicity)))){
    if(multiplicity>0)
      gl_Position=mvp*P[vertexIDCCW];
    if(multiplicity<0)
      gl_Position=mvp*P[vertexIDCW];
  }else
    gl_Position=vec4(0,0,0,0);
}
#endif
).";

std::string const _drawCapsVertexShaderSrc = R".(
#ifndef MAX_MULTIPLICITY
#define MAX_MULTIPLICITY 2
#endif//MAX_MULTIPLICITY

layout(location=0)in vec3 triangleVertexA;
layout(location=1)in vec3 triangleVertexB;
layout(location=2)in vec3 triangleVertexC;

uniform vec4 lightPosition    = vec4(10,10,10,1);
uniform mat4 modelMatrix      = mat4(1.f);
uniform mat4 viewMatrix       = mat4(1.f);
uniform mat4 projectionMatrix = mat4(1.f);

int greaterVec(vec3 a,vec3 b){
  return int(dot(sign(a-b),vec3(4.0,2.0,1.0)));
}

int computeMult(vec3 A,vec3 B,vec3 C,vec4 L){
  vec3 n=cross(C-A,L.xyz-A*L.w);
  return int(sign(dot(n,B-A)));
}

int currentMultiplicity(vec3 A,vec3 B,vec3 O,vec4 L){
  return int(sign(dot(cross(B-A,L.xyz-A),O-A)));
  if(greaterVec(A,O)>0)
    return computeMult(O,A,B,L);
  else if(greaterVec(B,O)>0)
    return -computeMult(A,O,B,L);
  else
    return computeMult(A,B,O,L);
}

void swap(inout vec3 A,inout vec3 B){vec3 C=A;A=B;B=C;}

void main(){
  vec3 A=triangleVertexA;
  vec3 B=triangleVertexB;
  vec3 C=triangleVertexC;
  if(greaterVec(A,B)>0)swap(A,B);
  if(greaterVec(B,C)>0)swap(B,C);
  if(greaterVec(A,B)>0)swap(A,B);
  int multiplicity=computeMult(A,B,C,lightPosition);
  if(multiplicity==0){gl_Position=vec4(0,0,0,1);return;}
  if(multiplicity<0)swap(A,B);

  float farCap = float(gl_InstanceID/2);
  int   vID    = ((farCap>0)?3-int(gl_VertexID):int(gl_VertexID));

  vec4 P=vec4(A*float(vID==0)+B*float(vID==1)+C*float(vID==2),1.0);
  
  P=P*(1.0-farCap)+vec4(P.xyz*lightPosition.w-lightPosition.xyz,0)*farCap;
  P=projectionMatrix*viewMatrix*modelMatrix*P;
  P=P.xyww*(1.0-farCap)+P*farCap;//front cap
  gl_Position=P;
}
).";
