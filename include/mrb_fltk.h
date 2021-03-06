#ifndef MRB_FLTK_H
#define MRB_FLTK_H

// =-=- GC Arena Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define ARENA_SAVE                   \
  int ai = mrb_gc_arena_save( mrb ); \
  if( ai == MRB_ARENA_SIZE )         \
    mrb_raise( mrb, E_RUNTIME_ERROR, "arena overflow" );

#define ARENA_RESTORE mrb_gc_arena_restore( mrb, ai );

// =-=- Context Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Defines a struct containing the fl instance, mrb instance, and mrb state named mrb_fltk_##name##_context
// Also defines the mrb datatype for the fl_instance wrapper
// This acts as a has_and_belongs_to relationship (Ruby <1-1> Context <1-1> FLTK)
#define CONTEXT_DEFINE( name, rb_class, fl_class ) \
  typedef struct {                                 \
    fl_class *fl_instance;                         \
    mrb_value rb_instance;                         \
    mrb_state *mrb;                                \
  } mrb_fltk_##name##_context;                     \
  static const struct mrb_data_type fltk_##name##_type = { #rb_class, mrb_fltk_free };

// Instance variable `context` is retrieved and unwrapped
// TODO: Make more flexible! Used in some methods (like setters)
#define CONTEXT_SETUP( name )                                                           \
  mrb_value context_value = mrb_iv_get( mrb, self, mrb_intern_cstr( mrb, "context" ) ); \
                                                                                        \
  mrb_fltk_##name##_context *context;                                                   \
  Data_Get_Struct( mrb, context_value, &fltk_##name##_type, context );

// Used within MRuby method functions
// Instance variable `context` will become an instance of the mrb_fltk_##name##_context struct
#define CONTEXT_CREATE( name, variable )                                                                            \
  mrb_fltk_##name##_context *variable = (mrb_fltk_##name##_context *)malloc( sizeof( mrb_fltk_##name##_context ) ); \
                                                                                                                    \
  if( !variable )                                                                                                   \
    mrb_raise( mrb, E_RUNTIME_ERROR, "can't alloc memory" );                                                        \
                                                                                                                    \
  memset( variable, 0, sizeof( mrb_fltk_##name##_context ) );                                                       \
  variable->rb_instance = self;                                                                                     \
  variable->mrb = mrb;

// Used within MRuby method functions
#define CONTEXT_WRAP( name, variable ) mrb_obj_value( Data_Wrap_Struct( mrb, mrb->object_class, &fltk_##name##_type, (void *)variable ) );

// Used within MRuby method functions
// Sets the @context instance variable to the wrapped context stuct instance (wrapped by CONTEXT_WRAP) from the `variable`
#define CONTEXT_SET_INSTANCE_VARIABLE( name, variable ) mrb_iv_set( mrb, self, mrb_intern_cstr( mrb, "context" ), CONTEXT_WRAP( name, variable ) );

// =-=- Widget Macro =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// TODO Move to widget.h
// Creates an initialize method for the class with the name mrb_fltk_##name##_initialize
#define IMPLEMENT_WIDGET_INITIALIZE_METHOD( name, type )                                                                                                                                      \
  static mrb_value mrb_fltk_##name##_initialize_method( mrb_state *mrb, mrb_value self ) {                                                                                                    \
    mrb_value *argv;                                                                                                                                                                          \
    int argc;                                                                                                                                                                                 \
                                                                                                                                                                                              \
    mrb_get_args( mrb, "*", &argv, &argc );                                                                                                                                                   \
                                                                                                                                                                                              \
    CONTEXT_CREATE( widget, context );                                                                                                                                                        \
                                                                                                                                                                                              \
    if( mrb_fltk_arg_check( "o", argc, argv ) ) {                                                                                                                                             \
      if( strcmp( mrb_obj_classname( mrb, argv[0] ), "Widget" ) )                                                                                                                             \
        mrb_raise( mrb, E_ARGUMENT_ERROR, "invalid argument" );                                                                                                                               \
      mrb_iv_set( mrb, self, mrb_intern_cstr( mrb, "context" ), argv[0] );                                                                                                                    \
                                                                                                                                                                                              \
      return self;                                                                                                                                                                            \
    } else if( mrb_fltk_arg_check( "iiii", argc, argv ) ) {                                                                                                                                   \
      context->fl_instance = (Fl_Widget *)new type( (int)mrb_fixnum( argv[0] ), (int)mrb_fixnum( argv[1] ), (int)mrb_fixnum( argv[2] ), (int)mrb_fixnum( argv[3] ) );                         \
    } else if( mrb_fltk_arg_check( "iiiis", argc, argv ) ) {                                                                                                                                  \
      context->fl_instance = (Fl_Widget *)new type( (int)mrb_fixnum( argv[0] ), (int)mrb_fixnum( argv[1] ), (int)mrb_fixnum( argv[2] ), (int)mrb_fixnum( argv[3] ), RSTRING_PTR( argv[4] ) ); \
    } else {                                                                                                                                                                                  \
      mrb_raise( mrb, E_ARGUMENT_ERROR, "invalid argument" );                                                                                                                                 \
    }                                                                                                                                                                                         \
                                                                                                                                                                                              \
    CONTEXT_SET_INSTANCE_VARIABLE( widget, context );                                                                                                                                         \
                                                                                                                                                                                              \
    return self;                                                                                                                                                                              \
  }

// =-=- Window Macro =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// TODO Move to window.h
// Creates an initialize method for the class with the name mrb_fltk_##name##_initialize
#define IMPLEMENT_WINDOW_INITIALIZE_METHOD( name, type )                                                                                                                                      \
  static mrb_value mrb_fltk_##name##_initialize_method( mrb_state *mrb, mrb_value self ) {                                                                                                    \
    mrb_value *argv;                                                                                                                                                                          \
    int argc;                                                                                                                                                                                 \
    mrb_get_args( mrb, "*", &argv, &argc );                                                                                                                                                   \
                                                                                                                                                                                              \
    CONTEXT_CREATE( widget, context );                                                                                                                                                        \
                                                                                                                                                                                              \
    if( mrb_fltk_arg_check( "iis", argc, argv ) ) {                                                                                                                                           \
      context->fl_instance = (Fl_Widget *)new type( (int)mrb_fixnum( argv[0] ), (int)mrb_fixnum( argv[1] ), RSTRING_PTR( argv[2] ) );                                                         \
    } else if( mrb_fltk_arg_check( "iiiis", argc, argv ) ) {                                                                                                                                  \
      context->fl_instance = (Fl_Widget *)new type( (int)mrb_fixnum( argv[0] ), (int)mrb_fixnum( argv[1] ), (int)mrb_fixnum( argv[2] ), (int)mrb_fixnum( argv[3] ), RSTRING_PTR( argv[4] ) ); \
    } else {                                                                                                                                                                                  \
      mrb_raise( mrb, E_ARGUMENT_ERROR, "invalid argument" );                                                                                                                                 \
    }                                                                                                                                                                                         \
                                                                                                                                                                                              \
    CONTEXT_SET_INSTANCE_VARIABLE( widget, context );                                                                                                                                         \
                                                                                                                                                                                              \
    return self;                                                                                                                                                                              \
  }

// =-=- Hidden Object Macro  -=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
//   Has ambiguous argument names... also what is this?
//   Also the error messages suck ass

#define IMPLEMENT_HIDDEN_OBJECT_INITIALIZE_METHOD( name, type_name )                       \
  static mrb_value mrb_fltk_##name##_initialize_method( mrb_state *mrb, mrb_value self ) { \
    mrb_value arg = mrb_nil_value();                                                       \
    mrb_get_args( mrb, "|o", &arg );                                                       \
                                                                                           \
    if( !mrb_nil_p( arg ) ) {                                                              \
      if( strcmp( mrb_obj_classname( mrb, arg ), "fltk_" #type_name ) )                    \
        mrb_raise( mrb, E_RUNTIME_ERROR, "can't alloc fltk::" #name );                     \
                                                                                           \
      mrb_iv_set( mrb, self, mrb_intern_cstr( mrb, "context" ), arg );                     \
    } else                                                                                 \
      mrb_raise( mrb, E_RUNTIME_ERROR, "can't alloc fltk::" #name );                       \
                                                                                           \
    return self;                                                                           \
  }

// =-=- Constant Macro =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=

#define DEFINE_FIXNUM_CONSTANT( rb_name, fl_name, module ) mrb_define_const( mrb, module, #rb_name, mrb_fixnum( fl_name ) );

// =-=- Fixnum Attribute Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=

// Implements a reader fixnum attribute for the mrb_fltk_##name##_class Ruby class
#define IMPLEMENT_FIXNUM_ATTRIBUTE_READER( name, rb_method, fl_class, fl_method )                    \
  static mrb_value mrb_fltk_##name##_##rb_method##_getter_method( mrb_state *mrb, mrb_value self ) { \
    CONTEXT_SETUP( widget );                                                                         \
                                                                                                     \
    return mrb_fixnum_value( ( (fl_class *)context->fl_instance )->fl_method() )                     \
  }

// Defines a reader fixnum attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_FIXNUM_ATTRIBUTE_READER( name, rb_method ) \
  mrb_define_method( mrb, mrb_fltk_##name##_class, #rb_method, mrb_fltk_##name##_##rb_method##_getter_method, ARGS_NONE() );

// Implements a reader fixnum attribute for the mrb_fltk_##name##_class Ruby class
// TODO: Should return the new value
#define IMPLEMENT_FIXNUM_ATTRIBUTE_WRITER( name, rb_method, fl_class, fl_method )                    \
  static mrb_value mrb_fltk_##name##_##rb_method##_setter_method( mrb_state *mrb, mrb_value self ) { \
    CONTEXT_SETUP( widget );                                                                         \
                                                                                                     \
    mrb_value value;                                                                                 \
    mrb_get_args( mrb, "i", &value );                                                                \
                                                                                                     \
    ( (fl_class *)context->fl_instance )->fl_method( mrb_fixnum( value ) );                          \
                                                                                                     \
    return value;                                                                                    \
  }

// Defines a writer fixnum attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_FIXNUM_ATTRIBUTE_WRITER( name, rb_method ) \
  mrb_define_method( mrb, mrb_fltk_##name##_class, #rb_method "=", mrb_fltk_##name##_##rb_method##_setter_method, ARGS_REQ( 1 ) );

// Implements a reader/writer fixnum attribute on the mrb_fltk_##name##_class Ruby class
#define IMPLEMENT_FIXNUM_ATTRIBUTE_ACCESSOR( name, rb_method, fl_class, fl_method ) \
  IMPLEMENT_FIXNUM_ATTRIBUTE_READER( name, rb_method, fl_class, fl_method );        \
  IMPLEMENT_FIXNUM_ATTRIBUTE_WRITER( name, rb_method, fl_class, fl_method );

// Defines a reader/writer fixnum attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_FIXNUM_ATTRIBUTE_ACCESSOR( name, rb_method ) \
  DEFINE_FIXNUM_ATTRIBUTE_READER( name, rb_method );        \
  DEFINE_FIXNUM_ATTRIBUTE_WRITER( name, rb_method );

// =-=- String Attribute Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=

#define IMPLEMENT_STRING_ATTRIBUTE_READER( name, rb_method, fl_class, fl_method )                    \
  static mrb_value mrb_fltk_##name##_##rb_method##_getter_method( mrb_state *mrb, mrb_value self ) { \
    CONTEXT_SETUP( widget );                                                                         \
                                                                                                     \
    const char *value = ( (fl_class *)context->fl_instance )->fl_method();                           \
                                                                                                     \
    return value ? mrb_str_new_cstr( mrb, value ) : mrb_nil_value();                                 \
  }

// Defines a reader string attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_STRING_ATTRIBUTE_READER( name, rb_method ) \
  mrb_define_method( mrb, mrb_fltk_##name##_class, #rb_method, mrb_fltk_##name##_##rb_method##_getter_method, MRB_ARGS_NONE() );

#define IMPLEMENT_STRING_ATTRIBUTE_WRITER( name, rb_method, fl_class, fl_method )                    \
  static mrb_value mrb_fltk_##name##_##rb_method##_setter_method( mrb_state *mrb, mrb_value self ) { \
    CONTEXT_SETUP( widget );                                                                         \
                                                                                                     \
    mrb_value value;                                                                                 \
    mrb_get_args( mrb, "S", &value );                                                                \
                                                                                                     \
    ( (fl_class *)context->fl_instance )->fl_method( RSTRING_PTR( value ) );                         \
                                                                                                     \
    return value;                                                                                    \
  }

// Defines a writer string attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_STRING_ATTRIBUTE_WRITER( name, rb_method, fl_class, fl_method ) \
  mrb_define_method( mrb, mrb_fltk_##name##_class, #rb_method "=", mrb_fltk_##name##_##rb_method##_setter_method, ARGS_REQ( 1 ) );

// Implements a reader/writer string attribute on the mrb_fltk_##name##_class Ruby class
#define IMPLEMENT_STRING_ATTRIBUTE_ACCESSOR( name, rb_method, fl_class, fl_method ) \
  IMPLEMENT_STRING_ATTRIBUTE_READER( name, rb_method, fl_class, fl_method );        \
  IMPLEMENT_STRING_ATTRIBUTE_WRITER( name, rb_method, fl_class, fl_method );

// Defines a reader/writer string attribute on the mrb_fltk_##name##_class Ruby class
#define DEFINE_STRING_ATTRIBUTE_ACCESSOR( name, rb_method ) \
  DEFINE_STRING_ATTRIBUTE_READER( name, rb_method );        \
  DEFINE_STRING_ATTRIBUTE_WRITER( name, rb_method );

// =-=- Method Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Defines an instance method on the mrb_fltk_##name##_class Ruby class
// rb_method is the name of the Ruby method which will be mrb_fltk_##name##_##rb_method##_method
// rb_method is the name of the Ruby method
// arguments is ARGS_NONE(), ARGS_REQ(1), etc...
#define DEFINE_INSTANCE_METHOD( name, rb_method, arguments ) \
  mrb_define_method( mrb, mrb_fltk_##name##_class, #rb_method, mrb_fltk_##name##_##rb_method##_method, arguments );

// Defines a class method on the mrb_fltk_##name##_class Ruby class
// rb_method is the name of the Ruby method which will be mrb_fltk_##name##_##rb_method##_class_method
// arguments is ARGS_NONE(), ARGS_REQ(1), etc...
#define DEFINE_CLASS_METHOD( name, rb_method, arguments ) \
  mrb_define_class_method( mrb, mrb_fltk_##name##_class, #rb_method, mrb_fltk_##name##_##rb_method##_class_method, arguments );

// =-=- Class/Module Macros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Defines a class with the name mrb_fltk_##name##_class
#define DEFINE_CLASS( name, type, super ) \
  struct RClass *mrb_fltk_##name##_class = mrb_define_class_under( mrb, mrb_fltk_class, #type, #super );

// =-=- Function Declarations =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static void mrb_fltk_free( mrb_state *mrb, void *p );

#endif // MRB_FLTK_H
