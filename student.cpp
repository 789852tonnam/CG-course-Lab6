#include "utils/Student.h"

#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

const std::vector<Vertex> TRIANGLE = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

VkVertexInputBindingDescription Vertex::bindingDescription() {
  VkVertexInputBindingDescription desc{};
  desc.binding = 0;
  desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  desc.stride = sizeof(Vertex);
  return desc;
}

std::vector<VkVertexInputAttributeDescription> Vertex::attributeDescriptions() {
  return {
      {0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
      {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
  };
}

struct Params {
  glm::mat4 mvp;
  std::uint32_t costLoops;
  std::uint32_t stripeWidth;
  std::uint32_t padded[2];
};

static_assert(sizeof(Params) == 80,
              "Params must match the std140 table in the handout");

std::vector<std::uint8_t> uniformBlock(std::uint32_t costLoops,
                                       std::uint32_t stripeWidth) {
  Params params{};
  //TODO2b
  params.mvp = glm::mat4(1.0f);
  params.costLoops = costLoops;
  params.stripeWidth = stripeWidth;

  std::vector<std::uint8_t> bytes(sizeof(Params));
  std::memcpy(bytes.data(), &params, sizeof(Params));
  return bytes;
}

PipelineState pipelineState(Variant v) {
  PipelineState s{};
  // TODO(TASK 3a): match the winding you chose in TASK 1a
  s.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  // TODO(TASK 3b): cull back faces
  s.cullMode = VK_CULL_MODE_BACK_BIT;
  // TODO(TASK 3c): depth compare, using Lab 05's reversed-Z convention
  s.depthCompare = VK_COMPARE_OP_GREATER;
  // TODO(TASK 3d): should this pipeline write depth?
  s.depthWrite = true;
  // TODO(TASK 3e): "earlyz_a.frag" or "earlyz_b.frag", chosen from v
  s.fragShader = (v == Variant::EarlyZFrontToBack || v == Variant::EarlyZBackToFront)
                     ? "earlyz_a.frag"
                     : "earlyz_b.frag";
  // TODO(TASK 3f): DrawOrder::FrontToBack or ::BackToFront, chosen from v
  s.drawOrder = (v == Variant::EarlyZFrontToBack || v == Variant::WriteDepthFrontToBack)
                    ? DrawOrder::FrontToBack
                    : DrawOrder::BackToFront;
  return s;
}

const std::vector<Vertex> QUAD = {
    // TODO(TASK 4a): four corners. Check values are in Part V of the handout.
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
};

const std::vector<std::uint16_t> QUAD_INDICES = {
    // TODO(TASK 4b): six indices, two triangles, both wound like TASK 1a.
    0, 2, 1,
    0, 3, 2,
};

static_assert(sizeof(glm::mat4) == 64);

std::vector<glm::mat4> instanceBuffer() {
  // TODO(TASK 5a): INSTANCES transforms, glm::translate and glm::scale.
  std::vector<glm::mat4> transforms;
  transforms.reserve(INSTANCES);
  for (std::uint32_t i = 0; i < INSTANCES; ++i) {
    const std::uint32_t col = i % 100;
    const std::uint32_t row = i / 100;
    const float x = -1.0f + (col + 0.5f) * 0.02f;
    const float y = -1.0f + (row + 0.5f) * 0.04f;
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    transforms.push_back(glm::scale(translation, glm::vec3(0.016f)));
  }
  return transforms;
}

void recordDraw(VkCommandBuffer cmd, std::uint32_t indexCount) {
  // TODO(TASK 5b): one vkCmdDrawIndexed, with an instance count.
  vkCmdDrawIndexed(cmd, indexCount, INSTANCES, 0, 0, 0);
}
