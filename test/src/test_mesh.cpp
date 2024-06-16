
#include <creator.hpp>
#include <gtest/gtest.h>
#include <memory_resource>
#include <rtr/header.hpp>
#include <rtr/mesh.hpp>
#include <rtr/write_mesh.hpp>

TEST(Mesh, BasicIntegration) {
    creator alloc(1024 * 1024 * 1024);
    void*   fileData = alloc.data();

    // Quickly make a test file
    {
        rtr::RootHeader* root = alloc.construct<rtr::RootHeader>();
        root->headers = alloc.constructArray<decodeless::offset_ptr<decodeless::Header>>(1);

        std::vector<glm::uvec3> triangleVertices = {{0, 1, 2}, {2, 3, 0}};
        std::vector<glm::vec3>  vertexPositions = {
            {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
        std::vector<glm::vec3> vertexNormals = {
            {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}};
        std::vector<glm::vec2> vertexTexCoords0 = {
            {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        std::vector<glm::vec4> vertexTangents = {{1.0f, 0.0f, 0.0f, 0.0f},
                                                 {1.0f, 0.0f, 0.0f, 0.0f},
                                                 {1.0f, 0.0f, 0.0f, 0.0f},
                                                 {1.0f, 0.0f, 0.0f, 0.0f}};

        std::vector<rtr::common::Mesh> meshes{{
            .triangleVertices = triangleVertices,
            .vertexPositions = vertexPositions,
            .vertexNormals = vertexNormals,
            .vertexTexCoords0 = vertexTexCoords0,
            .vertexTangents = vertexTangents,
        }};
        std::vector<std::string>       meshNames{"testmesh"};
        root->headers[0] = rtr::common::createMeshHeader(alloc.allocator(), meshes, meshNames);
    }

    // "load" the file - zero computation here
    const rtr::RootHeader* rtrFile = reinterpret_cast<const rtr::RootHeader*>(fileData);

    // Find the mesh header (binary searches and verifies the version number)
    const rtr::common::MeshHeader* meshHeader = rtrFile->findSupported<const rtr::common::MeshHeader>();
    ASSERT_NE(meshHeader, nullptr);

    // Access only the structured data needed, immediately without parsing,
    // decoding or deserializing anything. Memory may be paged in when accessed
    // by the thread.
    glm::uvec3 tri1 = meshHeader->meshes[0].triangleVertices[1];
    glm::uvec3 tri1Expect{2, 3, 0};
    EXPECT_EQ(tri1, tri1Expect);
}
