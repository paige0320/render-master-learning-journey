// ============================================================
//  Render Master — Shadow Mapping (C++ 版)：陰影【邊玩邊講版】
//  兩次渲染：
//   Pass 1：從「光的視角」畫一遍，只記錄深度 → 陰影貼圖(深度圖)
//   Pass 2：正常畫，把每個像素轉到光座標比對深度 → 判斷是否在陰影裡
//  操作：WASD 移動、滑鼠轉頭、ESC 離開
// ============================================================
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

glm::vec3 camPos   = glm::vec3(0.0f, 4.0f, 12.0f);
glm::vec3 camFront = glm::vec3(0.0f, -0.2f,-1.0f);
glm::vec3 camUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = -12.0f;
float lastX = 500.0f, lastY = 340.0f;
bool  firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) { lastX=(float)xpos; lastY=(float)ypos; firstMouse=false; }
    float xo=(float)xpos-lastX, yo=lastY-(float)ypos; lastX=(float)xpos; lastY=(float)ypos;
    yaw += xo*0.1f; pitch += yo*0.1f;
    if (pitch> 89.f) pitch= 89.f; if (pitch< -89.f) pitch=-89.f;
    glm::vec3 d;
    d.x=cos(glm::radians(yaw))*cos(glm::radians(pitch));
    d.y=sin(glm::radians(pitch));
    d.z=sin(glm::radians(yaw))*cos(glm::radians(pitch));
    camFront=glm::normalize(d);
}
void processInput(GLFWwindow* w)
{
    if (glfwGetKey(w,GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(w,true);
    float sp=5.0f*deltaTime;
    if (glfwGetKey(w,GLFW_KEY_W)==GLFW_PRESS) camPos+=sp*camFront;
    if (glfwGetKey(w,GLFW_KEY_S)==GLFW_PRESS) camPos-=sp*camFront;
    if (glfwGetKey(w,GLFW_KEY_A)==GLFW_PRESS) camPos-=glm::normalize(glm::cross(camFront,camUp))*sp;
    if (glfwGetKey(w,GLFW_KEY_D)==GLFW_PRESS) camPos+=glm::normalize(glm::cross(camFront,camUp))*sp;
}

float cubeVerts[] = {
    -0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,-0.5f,-0.5f, 0,0,-1,  0.5f,0.5f,-0.5f, 0,0,-1,
     0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,0.5f,-0.5f, 0,0,-1, -0.5f,-0.5f,-0.5f, 0,0,-1,
    -0.5f,-0.5f,0.5f, 0,0,1,  0.5f,-0.5f,0.5f, 0,0,1,  0.5f,0.5f,0.5f, 0,0,1,
     0.5f,0.5f,0.5f, 0,0,1, -0.5f,0.5f,0.5f, 0,0,1, -0.5f,-0.5f,0.5f, 0,0,1,
    -0.5f,0.5f,0.5f, -1,0,0, -0.5f,0.5f,-0.5f, -1,0,0, -0.5f,-0.5f,-0.5f, -1,0,0,
    -0.5f,-0.5f,-0.5f, -1,0,0, -0.5f,-0.5f,0.5f, -1,0,0, -0.5f,0.5f,0.5f, -1,0,0,
     0.5f,0.5f,0.5f, 1,0,0,  0.5f,0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,-0.5f, 1,0,0,
     0.5f,-0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,0.5f, 1,0,0,  0.5f,0.5f,0.5f, 1,0,0,
    -0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,0.5f, 0,-1,0,
     0.5f,-0.5f,0.5f, 0,-1,0, -0.5f,-0.5f,0.5f, 0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,
    -0.5f,0.5f,-0.5f, 0,1,0,  0.5f,0.5f,-0.5f, 0,1,0,  0.5f,0.5f,0.5f, 0,1,0,
     0.5f,0.5f,0.5f, 0,1,0, -0.5f,0.5f,0.5f, 0,1,0, -0.5f,0.5f,-0.5f, 0,1,0,
};

struct Mesh { unsigned int vao; int indexCount; };
Mesh makeMesh(const std::vector<float>& v, const std::vector<unsigned int>& idx)
{
    unsigned int vao,vbo,ebo;
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); glGenBuffers(1,&ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(float),v.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*sizeof(unsigned int),idx.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float))); glEnableVertexAttribArray(1);
    return { vao, (int)idx.size() };
}
Mesh makeCube(){ std::vector<float> v(cubeVerts,cubeVerts+sizeof(cubeVerts)/sizeof(float));
    std::vector<unsigned int> idx(36); for(unsigned i=0;i<36;i++) idx[i]=i; return makeMesh(v,idx); }
Mesh makeSphere(int st,int sl){ const float PI=3.14159265f; std::vector<float> v; std::vector<unsigned int> idx;
    for(int i=0;i<=st;i++){ float phi=PI*i/st; for(int j=0;j<=sl;j++){ float th=2*PI*j/sl;
        float nx=sinf(phi)*cosf(th),ny=cosf(phi),nz=sinf(phi)*sinf(th);
        v.insert(v.end(),{nx,ny,nz,nx,ny,nz}); } }
    int cols=sl+1; for(int i=0;i<st;i++)for(int j=0;j<sl;j++){int a=i*cols+j,b=a+cols;
        idx.insert(idx.end(),{(unsigned)a,(unsigned)b,(unsigned)(a+1),(unsigned)(a+1),(unsigned)b,(unsigned)(b+1)});}
    return makeMesh(v,idx); }
std::vector<Mesh> loadModel(const std::string& path, glm::vec3& center, float& maxExtent)
{
    std::vector<Mesh> meshes; Assimp::Importer imp;
    const aiScene* sc = imp.ReadFile(path, aiProcess_Triangulate|aiProcess_GenSmoothNormals|aiProcess_JoinIdenticalVertices);
    if(!sc||(sc->mFlags&AI_SCENE_FLAGS_INCOMPLETE)||!sc->mRootNode){ std::cerr<<"模型載入失敗: "<<imp.GetErrorString()<<"\n"; return meshes; }
    glm::vec3 mn(1e9f),mx(-1e9f);
    for(unsigned m=0;m<sc->mNumMeshes;m++){ aiMesh* me=sc->mMeshes[m]; std::vector<float> v; std::vector<unsigned int> idx;
        for(unsigned i=0;i<me->mNumVertices;i++){ aiVector3D p=me->mVertices[i];
            aiVector3D n=me->HasNormals()?me->mNormals[i]:aiVector3D(0,1,0);
            v.insert(v.end(),{p.x,p.y,p.z,n.x,n.y,n.z});
            mn=glm::min(mn,glm::vec3(p.x,p.y,p.z)); mx=glm::max(mx,glm::vec3(p.x,p.y,p.z)); }
        for(unsigned f=0;f<me->mNumFaces;f++) for(unsigned k=0;k<me->mFaces[f].mNumIndices;k++) idx.push_back(me->mFaces[f].mIndices[k]);
        meshes.push_back(makeMesh(v,idx)); }
    center=(mn+mx)*0.5f; glm::vec3 s=mx-mn; maxExtent=glm::max(s.x,glm::max(s.y,s.z)); return meshes;
}

const char* depthVert = R"(#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uLightSpace; uniform mat4 uModel;
void main(){ gl_Position = uLightSpace * uModel * vec4(aPos,1.0); })";
const char* depthFrag = R"(#version 330 core
void main(){ })";

const char* mainVert = R"(#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uModel,uView,uProj,uLightSpace;
out vec3 vN; out vec3 vFrag; out vec4 vFragLight;
void main(){
    vFrag = vec3(uModel*vec4(aPos,1.0));
    vN = mat3(transpose(inverse(uModel)))*aNormal;
    vFragLight = uLightSpace * vec4(vFrag,1.0);
    gl_Position = uProj*uView*vec4(vFrag,1.0);
})";
const char* mainFrag = R"(#version 330 core
in vec3 vN; in vec3 vFrag; in vec4 vFragLight;
out vec4 FragColor;
uniform vec3 uLightDir, uViewPos, uColor;
uniform sampler2D uShadowMap;
uniform float uBias;      // ← 邊玩邊講：可以把它調成 0 看 shadow acne
uniform int uPCF;         // ← 1=柔化 0=硬邊
float calcShadow(vec4 fragLight, vec3 n, vec3 l){
    vec3 proj = fragLight.xyz / fragLight.w;
    proj = proj*0.5+0.5;
    if(proj.z>1.0) return 0.0;
    float current = proj.z;
    float bias = max(uBias*(1.0-dot(n,l)), uBias*0.3);
    if(uPCF==0){ float d=texture(uShadowMap,proj.xy).r; return current-bias>d?1.0:0.0; }
    float sh=0.0; vec2 texel=1.0/vec2(textureSize(uShadowMap,0));
    for(int x=-1;x<=1;x++) for(int y=-1;y<=1;y++){
        float d=texture(uShadowMap,proj.xy+vec2(x,y)*texel).r;
        sh += (current-bias>d)?1.0:0.0;
    }
    return sh/9.0;
}
void main(){
    vec3 n=normalize(vN); vec3 l=normalize(-uLightDir);
    float diff=max(dot(n,l),0.0);
    vec3 viewDir=normalize(uViewPos-vFrag);
    float spec=pow(max(dot(viewDir,reflect(-l,n)),0.0),32.0);
    float shadow=calcShadow(vFragLight,n,l);
    vec3 color=(0.25 + (1.0-shadow)*(diff+0.4*spec))*uColor;
    FragColor=vec4(color,1.0);
})";

void checkShader(unsigned int s,const char* n){ int ok; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){char log[512];glGetShaderInfoLog(s,512,nullptr,log);std::cerr<<n<<" shader 失敗:\n"<<log<<"\n";} }
unsigned int makeProgram(const char* v_,const char* f_){
    unsigned int v=glCreateShader(GL_VERTEX_SHADER);glShaderSource(v,1,&v_,nullptr);glCompileShader(v);checkShader(v,"vertex");
    unsigned int f=glCreateShader(GL_FRAGMENT_SHADER);glShaderSource(f,1,&f_,nullptr);glCompileShader(f);checkShader(f,"fragment");
    unsigned int p=glCreateProgram();glAttachShader(p,v);glAttachShader(p,f);glLinkProgram(p);glDeleteShader(v);glDeleteShader(f);return p;
}

glm::vec3 boxPos[] = { {-3,0.5f,-1},{2,1.5f,2},{0,1.0f,-3},{3.5f,0.8f,-1.5f},{-2,2.0f,3},{4,0.5f,1} };
glm::vec3 boxCol[] = { {0.9f,0.4f,0.4f},{0.4f,0.8f,0.5f},{0.5f,0.6f,1.0f},{0.95f,0.8f,0.3f},{0.8f,0.5f,0.9f},{0.4f,0.85f,0.85f} };

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(1000,680,"Shadow Mapping - 邊玩邊講 (C++)  [WASD/Mouse, ESC]",nullptr,nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window,mouse_callback);
    glEnable(GL_DEPTH_TEST);
    std::cout<<"太陽會繞圈，物體互相投影。WASD 移動、滑鼠轉頭、ESC 離開\n";

    unsigned int depthProg=makeProgram(depthVert,depthFrag);
    unsigned int mainProg =makeProgram(mainVert, mainFrag);

    Mesh cube=makeCube(), sphere=makeSphere(24,32);
    glm::vec3 modelCenter; float modelExtent=1.0f;
    std::vector<Mesh> spider=loadModel(MODEL_FILE, modelCenter, modelExtent);
    float modelScale=(modelExtent>0.0001f)?(3.0f/modelExtent):1.0f;

    const unsigned int SHADOW=2048;
    unsigned int depthFBO; glGenFramebuffers(1,&depthFBO);
    unsigned int depthMap; glGenTextures(1,&depthMap);
    glBindTexture(GL_TEXTURE_2D,depthMap);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,SHADOW,SHADOW,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    float border[]={1.0f,1.0f,1.0f,1.0f}; glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);
    glBindFramebuffer(GL_FRAMEBUFFER,depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthMap,0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) std::cerr<<"FBO 不完整\n";
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    auto drawMesh=[&](const Mesh& m){ glBindVertexArray(m.vao); glDrawElements(GL_TRIANGLES,m.indexCount,GL_UNSIGNED_INT,0); };
    auto renderScene=[&](unsigned int prog,int modelLoc,int colorLoc){
        auto setM=[&](const glm::mat4& m){ glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(m)); };
        auto setC=[&](glm::vec3 c){ if(colorLoc>=0) glUniform3fv(colorLoc,1,glm::value_ptr(c)); };
        setC(glm::vec3(0.7f)); setM(glm::scale(glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0)),glm::vec3(24,0.4f,24))); drawMesh(cube);
        for(int i=0;i<6;i++){ setC(boxCol[i]);
            glm::mat4 m=glm::translate(glm::mat4(1.0f),boxPos[i]); m=glm::rotate(m,(float)i,glm::vec3(0,1,0)); setM(m); drawMesh(cube); }
        setC(glm::vec3(0.9f,0.6f,0.4f)); setM(glm::translate(glm::mat4(1.0f),glm::vec3(-4,1.2f,-2))); drawMesh(sphere);
        setC(glm::vec3(0.75f,0.72f,0.68f));
        glm::mat4 sm=glm::translate(glm::mat4(1.0f),glm::vec3(1.5f,0.2f,1.5f));
        sm=glm::scale(sm,glm::vec3(modelScale)); sm=glm::translate(sm,glm::vec3(-modelCenter.x,-modelCenter.y,-modelCenter.z));
        setM(sm); for(const Mesh& me:spider) drawMesh(me);
    };

    // ★ 邊玩邊講的可調參數（用鍵盤切）
    float bias = 0.004f; int pcf = 1;

    while(!glfwWindowShouldClose(window))
    {
        float now=(float)glfwGetTime();
        deltaTime=now-lastFrame; lastFrame=now;
        processInput(window);

        // B 鍵：切換 bias（0 = 看 shadow acne）；P 鍵：切換 PCF 柔化
        static bool pb=false, pp=false;
        bool nb=glfwGetKey(window,GLFW_KEY_B)==GLFW_PRESS;
        bool np=glfwGetKey(window,GLFW_KEY_P)==GLFW_PRESS;
        if(nb&&!pb){ bias=(bias>0.0f?0.0f:0.004f); std::cout<<"bias = "<<bias<<(bias==0?"  → 看看表面雜紋(shadow acne)":"  → 正常")<<"\n"; }
        if(np&&!pp){ pcf=1-pcf; std::cout<<"PCF = "<<(pcf?"ON 柔邊":"OFF 硬邊鋸齒")<<"\n"; }
        pb=nb; pp=np;

        float a=now*0.25f;
        glm::vec3 lightDir=glm::normalize(glm::vec3(sinf(a)*0.6f,-1.0f,cosf(a)*0.6f));
        glm::vec3 lightPos=-lightDir*20.0f;
        glm::mat4 lightProj=glm::ortho(-16.0f,16.0f,-16.0f,16.0f,1.0f,45.0f);
        glm::mat4 lightView=glm::lookAt(lightPos,glm::vec3(0.0f),glm::vec3(0,1,0));
        glm::mat4 lightSpace=lightProj*lightView;

        // Pass 1：從光的視角畫深度
        glViewport(0,0,SHADOW,SHADOW);
        glBindFramebuffer(GL_FRAMEBUFFER,depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(depthProg);
        glUniformMatrix4fv(glGetUniformLocation(depthProg,"uLightSpace"),1,GL_FALSE,glm::value_ptr(lightSpace));
        renderScene(depthProg, glGetUniformLocation(depthProg,"uModel"), -1);
        glBindFramebuffer(GL_FRAMEBUFFER,0);

        // Pass 2：正常畫 + 套陰影
        int fbW,fbH; glfwGetFramebufferSize(window,&fbW,&fbH);
        glViewport(0,0,fbW,fbH);
        glClearColor(0.05f,0.07f,0.10f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glUseProgram(mainProg);
        glm::mat4 view=glm::lookAt(camPos,camPos+camFront,camUp);
        glm::mat4 proj=glm::perspective(glm::radians(45.0f),(float)fbW/(float)fbH,0.1f,100.0f);
        glUniformMatrix4fv(glGetUniformLocation(mainProg,"uView"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(mainProg,"uProj"),1,GL_FALSE,glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(mainProg,"uLightSpace"),1,GL_FALSE,glm::value_ptr(lightSpace));
        glUniform3fv(glGetUniformLocation(mainProg,"uLightDir"),1,glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(mainProg,"uViewPos"),1,glm::value_ptr(camPos));
        glUniform1f(glGetUniformLocation(mainProg,"uBias"), bias);
        glUniform1i(glGetUniformLocation(mainProg,"uPCF"), pcf);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,depthMap);
        glUniform1i(glGetUniformLocation(mainProg,"uShadowMap"),0);
        renderScene(mainProg, glGetUniformLocation(mainProg,"uModel"), glGetUniformLocation(mainProg,"uColor"));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
