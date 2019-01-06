#ifndef O2_EXCEPTION_H
#define O2_EXCEPTION_H

#include <string>
#include <sstream>
#include <exception>

namespace O2 {
namespace exception {

class O2Exception : public std::exception {
public:
    /** Constructor (C strings).
     *  @param  message C-style string error message.
     *                  The string contents are copied upon construction.
     *                  Hence, responsibility for deleting the char* lies
     *                  with the caller. 
     *  @param file     C-style string which should be passed __FILE__.
     *  @param line     Integer which should be passed __LINE__.
     */
    explicit O2Exception(const char* message, const char* file, const int line) : msg_(message)
      {
      	msg_ += " ";
      	msg_ += std::string(file);
      	msg_ += ":";
      	msg_ += std::to_string(line);
      }

    /** Constructor (C++ STL strings).
     *  @param message  The error message.
     *  @param file     The __FILE__ from which the exception was caused.
     *  @param line     The __LINE__ from which the exception was caused.
     */
    explicit O2Exception(const std::string& message, const std::string& file, const int line) : msg_(message)
      {
      	msg_ += " ";
      	msg_ += file;
      	msg_ += ":";
      	msg_ += std::to_string(line);
      }

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~O2Exception() throw (){}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw (){
       return msg_.c_str();
    }
protected:
    /** Error message.
     */
    std::string msg_;
};

} // exception
} // O2

#endif /* O2_EXCEPTION_H */