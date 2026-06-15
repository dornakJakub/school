/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */ // if(type == CommandType::){ // 
    //   //nastav aktivní framebuffer
    //   mem.activatedFramebuffer = data.bindFramebufferCommand.id;
    // }

#include <studentSolution/gpu.hpp>
#include <algorithm>
#include <cmath>

glm::vec3 ndcToScreen(glm::vec4 const&p,uint32_t w,uint32_t h){
    glm::vec3 ndc = glm::vec3(p) / p.w;

    glm::vec3 out;

    out.x = (ndc.x * 0.5f + 0.5f) * (float)w;
    out.y = (ndc.y * 0.5f + 0.5f) * (float)h;
    out.z = ndc.z;

    return out;
}

float edgeFunction(glm::vec2 const&a,
                   glm::vec2 const&b,
                   glm::vec2 const&c){
    return
        (b.x-a.x)*(c.y-a.y)-
        (b.y-a.y)*(c.x-a.x);
}
void rasterizeTriangle(GPUMemory &mem, OutVertex const&v0, OutVertex const&v1, OutVertex const&v2) {
    auto&fbo = mem.framebuffers[mem.activatedFramebuffer];

    glm::vec3 p0 = ndcToScreen(v0.gl_Position, fbo.width, fbo.height);
    glm::vec3 p1 = ndcToScreen(v1.gl_Position, fbo.width, fbo.height);
    glm::vec3 p2 = ndcToScreen(v2.gl_Position, fbo.width, fbo.height);

    int minX = (int)std::floor(std::min({p0.x, p1.x, p2.x}));
    int maxX = (int)std::ceil (std::max({p0.x, p1.x, p2.x}));

    int minY = (int)std::floor(std::min({p0.y, p1.y, p2.y}));
    int maxY = (int)std::ceil (std::max({p0.y, p1.y, p2.y}));

    minX = std::max(minX, 0);
    minY = std::max(minY, 0);

    maxX = std::min(maxX,(int)fbo.width -1);
    maxY = std::min(maxY,(int)fbo.height-1);

    glm::vec2 A(p0.x, p0.y);
    glm::vec2 B(p1.x, p1.y);
    glm::vec2 C(p2.x, p2.y);

    float area = edgeFunction(A,B,C);

    if(area == 0.f)
        return;

    bool isCCW = area > 0.f;

    auto const& culling = mem.backfaceCulling;

    if(culling.enabled){

        bool frontFace =
            (isCCW == culling.frontFaceIsCounterClockWise);

        if(!frontFace)
            return;
    }

    for(int y=minY;y<=maxY;++y){
        for(int x=minX;x<=maxX;++x){

            glm::vec2 P(x+0.5f,y+0.5f);

            float w0 = edgeFunction(B,C,P);
            float w1 = edgeFunction(C,A,P);
            float w2 = edgeFunction(A,B,P);

            bool inside =
                (w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                (w0 <= 0 && w1 <= 0 && w2 <= 0);

            if(!inside)
                continue;

            w0 /= area;
            w1 /= area;
            w2 /= area;

            InFragment inFragment;

            float z0 = v0.gl_Position.z / v0.gl_Position.w;
            float z1 = v1.gl_Position.z / v1.gl_Position.w;
            float z2 = v2.gl_Position.z / v2.gl_Position.w;

            float depth =
                w0 * z0 +
                w1 * z1 +
                w2 * z2;

            inFragment.gl_FragCoord =
                glm::vec4(
                    x + 0.5f,
                    y + 0.5f,
                    depth,
                    1.f
                );

            OutFragment outFragment;

            ShaderInterface si;
            si.textures = mem.textures;
            si.uniforms = mem.uniforms;
            si.gl_DrawID = mem.gl_DrawID;

            auto fs =
                mem.programs[mem.activatedProgram].fragmentShader;

            if(fs)
                fs(outFragment,inFragment,si);

            if(fbo.color.data){

                uint8_t* pixel =
                    (uint8_t*)getPixel(fbo.color,x,y);

                pixel[0] =
                    (uint8_t)(glm::clamp(outFragment.gl_FragColor.r,0.f,1.f)*255.f);

                pixel[1] =
                    (uint8_t)(glm::clamp(outFragment.gl_FragColor.g,0.f,1.f)*255.f);

                pixel[2] =
                    (uint8_t)(glm::clamp(outFragment.gl_FragColor.b,0.f,1.f)*255.f);

                pixel[3] =
                    (uint8_t)(glm::clamp(outFragment.gl_FragColor.a,0.f,1.f)*255.f);
            }
        }
    }
}

void clearColor(GPUMemory &mem, ClearColorCommand cmd) {
  Framebuffer *fbo = mem.framebuffers + mem.activatedFramebuffer;

  if (fbo->color.data) {
    for (uint32_t y = 0; y < fbo->height; ++y) {
      for (uint32_t x = 0; x < fbo->width; ++x) {
        void *pixelStart = getPixel(fbo->color, x, y);
        for (uint32_t i = 0; i < fbo->color.channels; ++i) {
          uint8_t*pixelf = (uint8_t*)pixelStart;
          pixelf[i] = cmd.value[i] * 255;
        }
      }
    }
  }
}

void clearDepth(GPUMemory &mem, ClearDepthCommand cmd) {
  Framebuffer *fbo = mem.framebuffers + mem.activatedFramebuffer;

  if (fbo->depth.data) {
    for (uint32_t y = 0; y < fbo->height; ++y) {
      for (uint32_t x = 0; x < fbo->width; ++x) {
        void *pixelStart = getPixel(fbo->depth, x, y);
        for (uint32_t i = 0; i < fbo->depth.channels; ++i) {
          float*pixelf = (float*)pixelStart;
          pixelf[i] = cmd.value;
        }
      }
    }
  }
}

void clearStencil(GPUMemory &mem, ClearStencilCommand cmd) {
  Framebuffer *fbo = mem.framebuffers + mem.activatedFramebuffer;

  if (fbo->stencil.data) {
    for (uint32_t y = 0; y < fbo->height; ++y) {
      for (uint32_t x = 0; x < fbo->width; ++x) {
        void *pixelStart = getPixel(fbo->stencil, x, y);
        for (uint32_t i = 0; i < fbo->stencil.channels; ++i) {
          uint8_t*pixelf = (uint8_t*)pixelStart;
          pixelf[i] = cmd.value;
        }
      }
    }
  }
}

uint32_t computeVertexID(GPUMemory &mem, uint32_t i) {

  auto vao = mem.vertexArrays[mem.activatedVertexArray];

  if (vao.indexBufferID < 0)
    return i;

  Buffer indexBuffer = mem.buffers[vao.indexBufferID];

  if (vao.indexType == IndexType::U8) {
    uint8_t* ptr =
      (uint8_t*)((uint8_t*)indexBuffer.data + vao.indexOffset);

    return ptr[i];
  }

  if (vao.indexType == IndexType::U16) {
    uint16_t* ptr =
      (uint16_t*)((uint8_t*)indexBuffer.data + vao.indexOffset);

    return ptr[i];
  }

  if (vao.indexType == IndexType::U32) {
    uint32_t* ptr =
      (uint32_t*)((uint8_t*)indexBuffer.data + vao.indexOffset);

    return ptr[i];
  }

  return i;
}

void loadVertexAttributes(GPUMemory &mem, InVertex &inV, uint32_t vertexID) {
    VertexArray const& vao = mem.vertexArrays[mem.activatedVertexArray];

    for(uint32_t a = 0; a < maxAttribs; ++a){

        VertexAttrib const& attrib = vao.vertexAttrib[a];

        // nepouzity atribut
        if(attrib.type == AttribType::EMPTY)
            continue;

        // neplatny buffer
        if(attrib.bufferID < 0)
            continue;

        Buffer const& buffer =
            mem.buffers[attrib.bufferID];

        uint8_t const* ptr =
            (uint8_t const*)buffer.data +
            attrib.offset +
            attrib.stride * vertexID;

        switch(attrib.type){

            case AttribType::FLOAT:
                inV.attributes[a].v1 =
                    *(float const*)ptr;
            break;

            case AttribType::VEC2:
                inV.attributes[a].v2 =
                    *(glm::vec2 const*)ptr;
            break;

            case AttribType::VEC3:
                inV.attributes[a].v3 =
                    *(glm::vec3 const*)ptr;
            break;

            case AttribType::VEC4:
                inV.attributes[a].v4 =
                    *(glm::vec4 const*)ptr;
            break;

            case AttribType::UINT:
                inV.attributes[a].u1 =
                    *(uint32_t const*)ptr;
            break;

            case AttribType::UVEC2:
                inV.attributes[a].u2 =
                    *(glm::uvec2 const*)ptr;
            break;

            case AttribType::UVEC3:
                inV.attributes[a].u3 =
                    *(glm::uvec3 const*)ptr;
            break;

            case AttribType::UVEC4:
                inV.attributes[a].u4 =
                    *(glm::uvec4 const*)ptr;
            break;

            default:
            break;
        }
    }
}

// void draw(GPUMemory &mem, DrawCommand cmd) {
//   for (int i = 0; i < cmd.nofVertices; i++) {
//     VertexShader vSh = mem.programs[mem.activatedProgram].vertexShader;
//     if (mem.programs[mem.activatedProgram].vertexShader == nullptr)
//       return;
//     OutVertex outV;
//     ShaderInterface si;
//     si.textures = mem.textures;
//     si.uniforms = mem.uniforms;
//     si.gl_DrawID = mem.gl_DrawID;
//     InVertex inV;
//     uint32_t vertexID = computeVertexID(mem, i);
//     inV.gl_VertexID = vertexID;
//     loadVertexAttributes(mem, inV, vertexID);
//     vSh(outV, inV, si);
//   }
// }

void draw(GPUMemory &mem, DrawCommand cmd) {

    auto program =
        mem.programs[mem.activatedProgram];

    VertexShader vs = program.vertexShader;

    if(!vs)
        return;

    for(uint32_t t=0;t<cmd.nofVertices;t+=3){

        OutVertex outVertices[3];

        for(uint32_t v=0;v<3;++v){

            InVertex inVertex;

            uint32_t vertexID =
                computeVertexID(mem,t+v);

            inVertex.gl_VertexID = vertexID;

            loadVertexAttributes(mem,inVertex,vertexID);

            ShaderInterface si;
            si.textures = mem.textures;
            si.uniforms = mem.uniforms;
            si.gl_DrawID = mem.gl_DrawID;

            vs(outVertices[v],inVertex,si);
        }

        rasterizeTriangle(
            mem,
            outVertices[0],
            outVertices[1],
            outVertices[2]
        );
    }
}

//! [student_GPU_run]
void student_GPU_run(GPUMemory &mem,CommandBuffer const &cb) {
  (void)mem;
  (void)cb;
  mem.gl_DrawID = 0;

  for(uint32_t i = 0; i < cb.nofCommands; ++i){
    // typ a data příkazu
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::BIND_FRAMEBUFFER){ // pokud je to BindFramebufferCommand
      //nastav aktivní framebuffer
      mem.activatedFramebuffer = data.bindFramebufferCommand.id;
    }

    if(type == CommandType::BIND_PROGRAM){ 
      mem.activatedProgram = data.bindProgramCommand.id;
    }

    if(type == CommandType::BIND_VERTEXARRAY){ 
      mem.activatedVertexArray = data.bindVertexArrayCommand.id;
    }

    if(type == CommandType::BLOCK_WRITES_COMMAND){ 
      mem.blockWrites = data.blockWritesCommand.blockWrites;
    }

    if(type == CommandType::SET_BACKFACE_CULLING_COMMAND){ 
      mem.backfaceCulling.enabled = data.setBackfaceCullingCommand.enabled;
    }

    if(type == CommandType::SET_FRONT_FACE_COMMAND){ 
      mem.backfaceCulling.frontFaceIsCounterClockWise = data.setFrontFaceCommand.frontFaceIsCounterClockWise;
    }

    if(type == CommandType::SET_STENCIL_COMMAND){ 
      mem.stencilSettings = data.setStencilCommand.settings;
    }

    if(type == CommandType::SET_DRAW_ID){ 
      mem.gl_DrawID = data.setDrawIdCommand.id;
    }

    if(type == CommandType::CLEAR_COLOR){ 
      clearColor(mem, data.clearColorCommand);
    }

    if(type == CommandType::CLEAR_DEPTH){ 
      clearDepth(mem, data.clearDepthCommand);
    }
    
    if(type == CommandType::CLEAR_STENCIL){ 
      clearStencil(mem, data.clearStencilCommand);
    }

    if(type == CommandType::USER_COMMAND){ 
      data.userCommand.callback(data.userCommand.data);
    }

    if(type == CommandType::DRAW){ 
      draw(mem, data.drawCommand);
      mem.gl_DrawID++;
    }

    if(type == CommandType::SET_DRAW_ID){ 
      mem.gl_DrawID = data.setDrawIdCommand.id;
    }

    if(type == CommandType::SUB_COMMAND){ 
      student_GPU_run(mem, *data.subCommand.commandBuffer);
    }
  }

  /// \todo Tato funkce reprezentuje funkcionalitu grafické karty.<br>
  /// Měla by umět zpracovat command buffer, čistit framebuffer a kreslit.<br>
  /// mem obsahuje paměť grafické karty.
  /// cb obsahuje command buffer pro zpracování.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
  ///
  /// V základu jde o to, že cb obsahuje příkazy, které se musí provést nad pamětí mem.
  /// Správně fungující grafická karta dobře interpretuje příkazy v cb a správně změní obsah paměti mem.
}
//! [student_GPU_run]
