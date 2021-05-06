#ifndef MACROS_H
#define MACROS_H

#define _STR(x) #x
#define TODO(x) _Pragma(_STR(message("TODO: "_STR(x))))

#endif /* MACROS_H */
