#include <torch/custom_class.h>

#include <ATen/core/jit_type.h>

#include <atomic>
#include <unordered_map>

namespace torch {
namespace jit {

std::vector<c10::RegisterOperators>& registeredOps() {
  static std::vector<c10::RegisterOperators> ops;
  return ops;
}

std::unordered_map<std::string, at::ClassTypePtr>& customClasses() {
  static std::unordered_map<std::string, at::ClassTypePtr> customClasses;
  return customClasses;
}

void registerCustomClass(at::ClassTypePtr class_type) {
  TORCH_INTERNAL_ASSERT(class_type->name());
  auto name = class_type->name()->qualifiedName();
  TORCH_CHECK(!customClasses().count(name))
  customClasses()[name] = std::move(class_type);
}

at::ClassTypePtr getCustomClass(const std::string& name) {
  return customClasses().count(name) ? customClasses()[name] : nullptr;
}

bool isCustomClass(const c10::IValue& v) {
  return v.isObject() && v.toObject()->type()->name() &&
      getCustomClass(v.toObject()->type()->name()->qualifiedName());
}

std::unordered_map<std::string, std::shared_ptr<Function>>& customClassMethods() {
  static std::unordered_map<std::string, std::shared_ptr<Function>>
      customClassMethods;
  return customClassMethods;
}

Function* getCustomClassMethod(const std::string& qualname) {
  TORCH_CHECK(customClassMethods().count(qualname));
  return customClassMethods()[qualname].get();
}

void registerCustomClassMethod(std::shared_ptr<Function> fn) {
  TORCH_INTERNAL_ASSERT(
      !customClassMethods().count(fn->qualname().qualifiedName()));
  customClassMethods()[fn->qualname().qualifiedName()] = std::move(fn);
}

} // namespace jit
} // namespace torch
