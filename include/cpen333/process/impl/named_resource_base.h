/**
 * @file
 * @brief Base-class for named resources
 */
#ifndef CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H
#define CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H

#include <string>
#include "../../os.h"
#include "../named_resource.h"
#include "sha1.h"

// limit maximum resource name length to this
// Windows limit is 260, POSIX limit is 251, though OSX limit seems to be 30 :S
//     Windows names cannot contain a backslash '\'
//     POSIX names must begin with a / for portability if used between
//     multiple processes, otherwise behaviour is implementation defined,
//     and cannot contain a / anywhere else.
// Due to the 30 character limit on POSIX, we will take a sha1 hash and base64-encode
//      which will generate 28 characters.  Add a / prefix and terminating zero = 30.
//      We will also replace an / with a _ in the resulting hash

#ifdef CPEN333_USE_RAW_NAMES
/**
 * @brief Size required for resource unique identifier
 */
#define MAX_RESOURCE_NAME 250
#else
/**
 * @brief Size required for resource unique identifier
 */
#define MAX_RESOURCE_NAME 30
#endif

namespace cpen333 {
namespace process {

// implementation details
namespace impl {

/**
 * @brief Base-class for named resources
 *
 * Stores a unique identifier name
 */
class named_resource_base : public virtual named_resource {
 private:
  char name_[MAX_RESOURCE_NAME];

 protected:
  /**
   * @brief set's the resource's name
   * @param name
   */
  void set_name(const std::string &name) {
    make_resource_name(name, name_);
  }

  void set_name_raw(const char *name) {
    for (int i=0; i<MAX_RESOURCE_NAME; ++i) {
      name_[i] = name[i];
      if (name_[i] == 0) {
        break;
      }
    }
  }

 public:
  /**
   * @brief Constructor for base named resource
   * @param name unique identifier
   */
  named_resource_base(const std::string &name) {
    set_name(name);
  }

  // do not allow copying or moving
 private:
  named_resource_base(const named_resource_base &) DELETE_METHOD;
  named_resource_base(named_resource_base &&) DELETE_METHOD;
  named_resource_base &operator=(const named_resource_base &) DELETE_METHOD;
  named_resource_base &operator=(named_resource_base &&) DELETE_METHOD;

 public:
  /**
   * @brief Destructor, does nothing
   */
  virtual ~named_resource_base() {
    // clean-up
  }

  virtual bool unlink() = 0;  // abstract, force unlink to be defined

  /**
  * @copydoc cpen333::process::named_resource::unlink(const std::string&)
  */
  static bool unlink(const std::string &name) {
    UNUSED(name);
    return false;
  }

 protected:

  /**
   *  @brief Internal-use system name
   *  @return underlying unique identifier name
   */
  std::string name() const {
    return name_;
  }

  /**
   * @brief Internal-use system name as char pointer
   * @return pointer to underlying unique identifier name
   */
  const char *name_ptr() const {
    return &name_[0];
  }

  /**
   * @brief Creates a valid resource name
   *
   * Creates a valid resource name for the platform, on Linux/OSX this is
   * a leading '/' with sha1 base64-encoded hash of the string name.  We
   * will also replace any other '/' with '_'
   * @param name original resource name
   * @param out platform-safe resource name
   */
  static void make_resource_name(const std::string &name, char out[]) {

    // leading slash for pathname
    out[0] = '/';

#ifdef CPEN333_USE_RAW_NAMES
    // raw names useful for debugging
    for (size_t i=0; i<name.length(); ++i) {
      if (name[i] == '/') {
        out[i+1] = '_';
      } else {
        out[i+1] = name[i];
      }
    }
#else
    sha1 hash = sha1(name.c_str()).finalize();
    hash.print_base64(&out[1], true); // print starting at offset 1, with terminating zero
    // replace '/', '+', and '=' with _ for safer path name
    for (int i=1; i<MAX_RESOURCE_NAME; ++i) {
      if (out[i] == '/') {
        out[i] = '_';
      }
    }
#endif
  }

};

} // impl

} // process
} // cpen333

#endif //CPEN333_PROCESS_IMPL_NAMED_RESOURCE_H
