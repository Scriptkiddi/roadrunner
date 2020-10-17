/**
 * @file tel_macros.h
 * @brief Tellurium Plugins C API 2012-2014
 * @author Totte Karlsson & Herbert M Sauro
 *
 * <--------------------------------------------------------------
 * This file is part of cTellurium.
 * See http://code.google.com/p/roadrunnerlib for more details.
 *
 * Copyright (C) 2012-2014
 *   University of Washington, Seattle, WA, USA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * In plain english this means:
 *
 * You CAN freely download and use this software, in whole or in part, for personal,
 * company internal, or commercial purposes;
 *
 * You CAN use the software in packages or distributions that you create.
 *
 * You SHOULD include a copy of the license in any redistribution you may make;
 *
 * You are NOT required include the source of software, or of any modifications you may
 * have made to it, in any redistribution you may assemble that includes it.
 *
 * YOU CANNOT:
 *
 * redistribute any piece of this software without proper attribution;
*/
#ifndef tel_macrosH
#define tel_macrosH
//=================== MACROS ================================
/*!
 \brief macro to open a try/catch statement
 \ingroup macros
*/

#define start_try                                           \
        try                                                 \
        {

/*!
 \brief macro for catch statement in a routine returning a bool
 \ingroup macros
*/

#define  tel_catch_bool_macro                                   \
    }                                                       \
    catch(exception& ex)                                    \
    {                                                       \
        stringstream msg;                                   \
        msg<<"Tellurium exception: "<<ex.what();            \
        tpSetError(msg.str());                                \
        return false;                                       \
    }

/*!
 \brief macro for catch statement in a routine returning a pointer
 \ingroup macros
*/
#define  tel_catch_ptr_macro                                    \
    }                                                       \
    catch(exception& ex)                                    \
    {                                                       \
        stringstream msg;                                   \
        msg<<"Tellurium exception: "<<ex.what();            \
        tpSetError(msg.str());                                \
        return NULL;                                        \
    }

/*!
 \brief macro for catch statement in a routine returning a positive integer
 \ingroup macros
*/
#define tel_catch_int_macro                                     \
    }                                                       \
    catch(exception& ex)                                    \
    {                                                       \
        stringstream msg;                                   \
        msg<<"Tellurium exception: "<<ex.what();            \
        tpSetError(msg.str());                                \
        return -1;                                          \
    }

/*!
 \brief macro for catch statement in a routine returning an unsigned integer
 \ingroup macros
*/
#define tel_catch_uint_macro                                   \
    }                                                       \
    catch(exception& ex)                                    \
    {                                                       \
        stringstream msg;                                   \
        msg<<"Tellurium exception: "<<ex.what();            \
        tpSetError(msg.str());                                \
        return 0;                                          \
    }

/*!
 \brief macro for catch statement in a routine returning void
 \ingroup macros
*/
#define tel_catch_void_macro                                  \
    }                                                       \
    catch(const exception& ex)                              \
    {                                                       \
        stringstream msg;                                   \
        msg<<"Tellurium exception: "<<ex.what();            \
        tpSetError(msg.str());                                \
    }

#endif

