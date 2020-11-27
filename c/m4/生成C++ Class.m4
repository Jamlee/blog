define(`upcase', `translit(`$*', `a-z', `A-Z')')dnl
define(`lowcase', `translit(`$*', `A-Z', `a-z')')dnl
define(`define_an_object', ``'dnl
`#ifndef' upcase(`$1')_`'upcase(`$2')_H
`#define' upcase(`$1')_`'upcase(`$2')_H
 
`#define' upcase(`$1')_TYPE_`'upcase(`$2')`'dnl
      (lowcase(`$1')_`'lowcase(`$2')_get_type ())
`#define' upcase(`$1')_`'upcase(`$2')(object)`'dnl
      (G_TYPE_CHECK_INSTANCE_CAST((object), `'dnl
      upcase(`$1')_TYPE_`'upcase(`$2'), `$1'`$2'))
`#define' upcase(`$1')_IS_`'upcase(`$2')(object)`'dnl
      (G_TYPE_CHECK_INSTANCE_TYPE((object), `'dnl
      upcase(`$1')_TYPE_`'upcase(`$2'))
`#define' upcase(`$1')_`'upcase(`$2')_CLASS(klass)`'dnl
      (G_TYPE_CHECK_CLASS_CAST((klass), `'dnl
      upcase(`$1')_TYPE_`'upcase(`$2'), `$1'`$2'Class))
`#define' upcase(`$1')_IS_`'upcase(`$2')_CLASS(klass)`'dnl
      (G_TYPE_CHECK_CLASS_TYPE((klass), `'dnl
      upcase(`$1')_TYPE_`'upcase(`$2')))
`#define' upcase(`$1')_`'upcase(`$2')_GET_CLASS(object)`'dnl
      (G_TYPE_INSTANCE_GET_CLASS((object), `'dnl
      upcase(`$1')_TYPE_`'upcase(`$2'), `$1'`$2'Class))
 
typedef struct _`$1'`$2' `$1'`$2';
typedef struct _`$1'`$2'Class `$1'`$2'Class;
  
struct  _`$1'`$2'{
        GObject parent;
};
 
struct  _`$1'`$2'Class{
        GObjectClass parent_class;
};
 
GType lowcase(`$1')_`'lowcase(`$2')_get_type (void);
 
`#endif' /* upcase(`$1')_`'upcase(`$2')_H */')dnl

define_an_object(Foo, Object)