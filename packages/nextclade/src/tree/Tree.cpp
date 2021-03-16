#include "Tree.h"

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <string>

namespace Nextclade {
  class ErrorAuspiceJsonV2TreeNotFound : public std::runtime_error {
  public:
    explicit ErrorAuspiceJsonV2TreeNotFound()
        : std::runtime_error("Auspice Json v2: format is invalid: .tree is not found") {}
  };

  class TreeImpl {
    rapidjson::Document json;

  public:
    explicit TreeImpl(const std::string& auspiceJsonV2) {
      json.Parse(auspiceJsonV2.c_str());
    }

    const rapidjson::Value* get(const char* path) const {
      const rapidjson::Value* result = rapidjson::Pointer(path).Get(json);
      if (!result) {
        return nullptr;
      }
      return result;
    }

    rapidjson::Value* get(const char* path) {
      return const_cast<rapidjson::Value*>(// NOLINT(cppcoreguidelines-pro-type-const-cast)
        std::as_const(*this).get(path)     //
      );
    }

    rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator() {
      return json.GetAllocator();
    }

  };

  Tree::Tree(const std::string& auspiceJsonV2) : pimpl(std::make_unique<TreeImpl>(auspiceJsonV2)) {}

  Tree::~Tree() {}// NOLINT(modernize-use-equals-default)

  TreeNode Tree::root() const {
    auto* treeRootValue = pimpl->get("/tree");
    if (treeRootValue->IsNull()) {
      throw ErrorAuspiceJsonV2TreeNotFound();
    }
    return TreeNode{treeRootValue, &pimpl->allocator()};
  }
}// namespace Nextclade