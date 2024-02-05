# Ready To Render (RTR) mappable binary file format

Extensible 3D mesh file format for speed and convenience. Designed with the
https://github.com/decodeless philosophy, a RTR file can be "loaded" by type
casting.

- Header only, just structs and helpers
- Memory mappable (optional)
- Strongly typed
- Simple data, but extensible
- Similar to [glTF](https://www.khronos.org/gltf/), but more binary

```
#include <rtr/header.hpp>
#include <rtr/mesh.hpp>
...

// Read the data into memory or better yet, memory map with
// decodeless::file data("file.rtr");
void* data = ...;

auto root = reinterpret_cast<const rtr::RootHeader>(data);
assert(root->validate());  // check for binary compatibility

// Helper to search root->headers (no allocation or loading is done)
// You could trivially extend the file with your own headers!
auto meshHeader = root->findSupported<rtr::common::MeshHeader>();
assert(meshHeader);

// Read structured/relational data without decoding/copying/building an index
const rtr::common::Mesh& mesh0 = meshHeader->meshes[0];
glm::uvec3 tri1   = mesh0.triangleVertices[1];
glm::vec3  tri1v0 = mesh0.vertexPositions[tri1.x];
glm::vec3  tri1v1 = mesh0.vertexPositions[tri1.y];
glm::vec3  tri1v2 = mesh0.vertexPositions[tri1.z];
```

The `rtr::common::*` headers are intentionally simple and restrictive so that
code using them does not have to support many permutations of input data. I.e.
it is up to the file writer to conform to a simple standard.

To create and view RTR files, see https://github.com/pknowles/rtrtool.

## Cmake

This library is header-only so you can trivially add the include directories.
Alternatively, expose a cmake target with one of the following:

```
// Just add it directly (e.g. if it were a submodule)
add_subdirectory(readytorender)

// Search for it (the config file just calls add_subdirectory anyway)
find_package(readytorender REQUIRED CONFIG PATHS "${CMAKE_CURRENT_LIST_DIR}/path/to/readytorender/")

// Download at configure time
FetchContent_Declare(
    readytorender
    GIT_REPOSITORY https://github.com/pknowles/readytorender.git
    GIT_TAG pick_a_hash_or_release_tag)
FetchContent_MakeAvailable(readytorender)
```

## Limitations

Currently all data is in the one file. This prevents data reuse, for example
scenes that use the same mesh or meshes that use the same textures. While the
headers include rtr::Scene, this is intended to place instances of meshes to
form a single 3D model rather than be a whole scene description. A future
improvement may be a new scene level file to reference existing rtr files.

The widely used [GLM](https://github.com/g-truc/glm) library is used in the
header definitions for ease of use.

The simplicity achieved relies on assumed system architecture similarities. Some
systems have particular data addressing and alignment constraints that can
introduce differences in binary data layouts. For example, a file written on a
little endian system will appear corrupted when read on a big endian system.
However, most processes are little endian and some basic detection is in place
to gracefully error out in such cases. Other differences exist and the plan to
handle them is:

1. Make data types binary compatible when possible
2. Detect impossible cases and gracefully error out

## Contributing

Issues and pull requests are most welcome, thank you! Note the
[DCO](CONTRIBUTING) and MIT [LICENSE](LICENSE).
