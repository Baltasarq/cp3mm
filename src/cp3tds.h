#ifndef CP3TDS_H_INCLUDED
#define CP3TDS_H_INCLUDED

#include "stringman.h"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>

namespace Cp3mm {

namespace Tds {

/// Exception to be thrown due to semantic errors
class SemanticError : public std::runtime_error {
public:
    SemanticError(const char * msg)
        : std::runtime_error( msg )
        {}
};

/// Exception to be thrown due to strictness errors
class StrictnessError : public SemanticError {
public:
    StrictnessError(const char * msg)
        : SemanticError( msg )
        {}
};

/**
    Entity base class
    All items that need checking should inherit this mixin.
    It adds the strictness level and the obligation to define chkHigh(), chkMedium() and chkLow()
*/
class Entity {
private:
    std::string name;

public:
    /// The type of strictness level
    enum Strictness { LowStrictness, MediumStrictness, HighStrictness, ErroneousStrictness };

    /// The corresponding, human-readable strictness levels
    static const std::string StrStrictness[];

    /// Constructor of class entity
    /// @param n The name of the entity
    Entity(const std::string &n) : name( n )
        {}
    virtual ~Entity()
        {}

    /// The main checking function, checking the basics and calling the appropriate function,
    /// corresponding to the strictness level
    /// @see chkBasic, Strictness
    virtual void chk() const
        {
            chkBasic();

            if ( getStrictnessLevel() == LowStrictness ) {
                chkLow();
            }
            else
            if ( getStrictnessLevel() == MediumStrictness ) {
                chkMedium();
            }
            else chkHigh();
        }

    /// If there is something central for the item to be checked,
    /// then it should override this
    virtual void chkBasic() const
        {}

    /// Semantic checkings for the low level of strictness
    virtual void chkLow() const      = 0;

    /// Semantic checkings for the medium (default) level of strictness
    virtual void chkMedium() const   = 0;

    /// Semantic checkings for the high level of strictness
    virtual void chkHigh() const     = 0;

    /// Returns the current strictness level
    /// @return the strictness level in terms of the Strictness type
    /// @see Strictness
    static Strictness getStrictnessLevel()
        { return strictness; }

    /// Converts the current strictness level to a string
    /// @return A human-readable string explaining the strictness level
    static const std::string &cnvtStrictnessToString()
        { return cnvtStrictnessToString( strictness ); }

    /// Converts a given strictness level to a string
    /// @param strictness The strictness level to convert to a string
    /// @return A human-readable string explaining the strictness level
    /// @see Strictness, StrStrictness
    static const std::string &cnvtStrictnessToString(Strictness strictness);

    /// Sets the current strictness level
    /// @param value the new strictness level
    /// @see Strictness
    static void setStrictness(Strictness value)
        { strictness = ( ( value == ErroneousStrictness ) ? MediumStrictness : value ); }

    /// Gets the name of the entity
    /// @return The name, as std::string
    const std::string &getName() const
        { return name; }

protected:
    /// Sets the name of the entity
    /// @param n The new name, as std::string
    void setName(const std::string &n)
        { name = n; }

private:
    static Strictness strictness;
};


class Module;
class Member;

/// Containers are classes and namespaces
class Container : public Entity {
public:
    /// This type holds a list of namespaces in a module,
    /// and a list of classes in a namespace
    /// @see Namespace, Module
    typedef std::vector<Container *> ContainerList;

    /// This type holds a list of functions and constants in a Namespace
    /// and a list of methods and attributes in a class
    /// @see Namespace, Module
    typedef std::vector<Member *> MembersList;
private:
    /// To which container this container pertaints to? Class -> Namespace, Namespace -> Namespace
    Container * myContainer;
    /// The Module this class/namespace pertains to
    Module * myModule;
    /// The current visibility for items in classes and namespaces
    /// @see Member::Visibility
    const std::string * currentVisibility;
    /// The current container while defining subcontainers (subnamespaces)
    Container * currentContainer;
    /// The list of subcontainers
    /// @see ContainerList
    ContainerList containerList;
public:
    Container(Container *c, Module *m, const std::string &n = "")
        : Entity( n ), myContainer( c ), myModule( m ), currentContainer(NULL)
        {}

    virtual ~Container();

    /// Returns the owner of this container
    /// @see myContainer
    virtual Container * getContainer()
        { return myContainer; }

    /// Returns the modeule this container is in
    /// @see myModule
    Module * getModule()
        { return myModule; }

    /// Sets the current visibility, inside this container
    /// @see currentVisibility
    virtual void setCurrentVisibility(const std::string *v);

    /// Returns the current visibility, inside this container
    /// @see currentVisibility
    const std::string * getCurrentVisibility() const
        { return currentVisibility; }

    /// Returns the current container
    /// @see currentContainer
    Container * getCurrentContainer()
        { return currentContainer; }

    /// Returns the current container
    /// @see currentContainer
    const Container * getCurrentContainer() const
        { return currentContainer; }

    /// Adds a subcontainer to this container
    /// @see containerList
    Container * addContainer(Container *);

    /// Gets the list of subcontainers
    /// @see containerList
    ContainerList &getContainerList()
        { return containerList; }

    /// Adds a subcontainer to this container
    /// @see containerList
    const ContainerList &getContainerList() const
        { return containerList; }

    /// The current container which was being processed is finished,
    /// so it is time to "go up" to the parent one.
    /// @see currentContainer
    virtual Container * setOuterAsCurrentContainer() {
        return ( currentContainer = currentContainer->getContainer() );
    }

    /// Removes all members in a list of members
    /// @see MembersList
    static void removeVectorOfMembers(MembersList &l);
};

class Class;


/// This class is the parent class for
/// Constants/Functions and Attributes/Methods, inside a Container.
/// @see Container, Code, Data, Constant, Function, Attribute, Method
class Member : public Entity {
friend class Class;
friend class Namespace;
public:
    static const std::string AccessOperator;
    static const std::string DotOperator;
    static const std::string Semicolon;

    static const std::string StaticStorage;
    static const std::string InlineStorage;
    static const std::string VolatileStorage;
    static const std::string AutoStorage;
    static const std::string FriendStorage;

    /// List of all available storages
    static const std::string * Storage[];

    static const std::string VoidType;
    static const std::string IntType;
    static const std::string LongIntType;
    static const std::string DoubleType;
    static const std::string FloatType;
    static const std::string CharType;
    static const std::string BoolType;

    /// List of all available native types
    static const std::string * Type[];

    static const std::string MdfStatic;
    static const std::string MdfConst;
    static const std::string MdfVirtual;
    static const std::string MdfUnsigned;
    static const std::string MdfExplicit;
    static const std::string MdfIsPointer;
    static const std::string MdfIsReference;

    /// List of all available modifiers
    static const std::string * Modifier[];

    static const std::string PrivateVisibility;
    static const std::string ProtectedVisibility;
    static const std::string PublicVisibility;

    /// List of all available visibilities
    static const std::string * Visibility[];

    /// The entry point's name
    static const std::string MainFunctionId;

    /// Type for storing the modifiers applyable to this member
    /// It is a std::vector of std::string *
    typedef std::vector<const std::string *> Modifiers;

    virtual ~Member() {}

    Member(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Entity( n ), type( t ), storage( NULL ), myContainer( NULL ),
          visibility( NULL), pointer( false ), reference( false ), lineNumber( l )
        {}
    Member(unsigned int l, const std::string &n, const std::string &t)
        : Entity( n ), type( NULL ), userDefinedType(t), storage( NULL ),
          myContainer( NULL ), visibility( NULL), pointer( false ),
          reference( false ), lineNumber( l )
        {}

    /// Determines whether the type of the member is native (not user-defined) or not
    /// @return true if it is not native (it is user-defined), false otherwise
    /// @see getType
    bool isUserType() const
        { return ( type == NULL ); }

    /// Adds a new modifer to the list of modifiers for this member
    /// @param m The new modifier of the modifiers list
    /// @see Modifier
    void addModifier(const std::string *m)
        { modifiers.push_back( m ); }

    /// Each member must have a name that identifiers it uniquely, traversing all containers
    /// it is included in.
    /// @return An unique identifier in the form of container_name::container_name...::member_name
    virtual std::string getQualifiedName() = 0;

    /// Returns the type of the member, it does not distignuish between being native or not
    /// @return std::string with the type inside it
    /// @see isUserType, type, userDefinedType
    const std::string &getType() const
        { return ( type == NULL ) ? userDefinedType : ( *type ); }

    /// Returns the native type, which can be NULL if the type of this member is user-defined
    /// Returns the type of the member, it does not distignuish between being native or not
    /// @return pointer to const std::string with the type native tpe inside it
    /// @see isUserType
    const std::string *getSystemType() const
        { return type; }

    /// Returns the list of modifiers that were applied to this member
    /// @return A list of modifiers as a Modifiers object.
    /// @see Modifiers
    const Modifiers getModifiers() const
        { return modifiers; }

    /// Allows to modify the whole set of modifiers
    /// @param m A list of modifiers as a Modifiers object.
    /// @see Modifiers, modifiers
    void setModifiers(const Modifiers &m)
        { modifiers = m; }

    /// Checkes whether a modifier is int the list of modifiers
    /// @param m The modifier to check for, as std::string.
    /// @return a pointer to the std::string of the modifier
    /// @see Modifiers
    const std::string * lookForModifier(const std::string &m) const;

    /// Returns the list of modifiers that were applied to this member
    /// This is the list of modifiers to apply to the interface part of the member
    /// @return A list of modifiers as a Modifiers object.
    /// @see Modifiers
    virtual std::string getInterfaceModifiersAsString() const;

    /// Returns the list of modifiers that were applied to this member
    /// This is the list of modifiers to apply to the implementation part of the member
    /// @return A list of modifiers as a Modifiers object.
    /// @see Modifiers
    virtual std::string getImplementationModifiersAsString() const;

    /// Returns the storage of this member
    /// @return The storage as a pointer to const std::string
    /// @see Storage
    const std::string &getStorage() const
        { return *storage; }

    /// Returns the storage of this member
    /// @return The storage as a const std::string *
    /// @see Storage, storage
    const std::string *getSystemStorage() const
        { return storage; }

    /// Changes the storage of this member
    /// @param s The storage as a pointer to const std::string *
    /// @see Storage
    void setStorage(const std::string *s)
        { storage = s; }

    /// Returns the visibility of this member
    /// @return The visibility as a const std::string
    /// @see Visibility
    const std::string &getVisibility() const
        { return *visibility; }

    /// Returns the visibility of this member
    /// @return The visibility as a const std::string *
    /// @see Visibility, visibility
    const std::string *getSystemVisibility() const
        { return visibility; }

    /// Changes the visibility of this member
    /// @param v The visibility as a pointer to const std::string *
    /// @see Visibility
    void setVisibility(const std::string * v)
        { visibility = v; }

    /// Determines whether the member is static or not
    /// @return true if the member has static storage, false otherwise
    /// @see Storage, getStorage
    bool isStatic() const
        { return ( getSystemStorage() == &Member::StaticStorage ); }

    /// Determines whether the return type of the member is a pointer
    /// @return true if the type of the member is a pointer, false otherwise
    bool isPointer() const
        { return pointer; }

    /// Sets that the return type of the member is a pointer
    /// @param v true if the type of the member is to be a a pointer, false otherwise
    void setIsPointer(bool v)
        { pointer = v; }

    /// Determines whether the return type of the member is a reference
    /// @return true if the type of the member is a reference, false otherwise
    bool isReference() const
        { return reference; }

    /// Sets that the return type of the member is a reference
    /// @param v true if the type of the member is to be a a reference, false otherwise
    void setIsReference(bool v)
        { reference = v; }

    /// Returns the interface version for this member (a prototype or extern declaration)
    virtual std::string getPrototype()          = 0;

    /// Returns the inline version of the member (similar to the implementation version,
    /// except that it has all modifiers as well: prototype + body in case of functions)
    virtual std::string getInline()             = 0;

    /// Returns the implementation version of this member. This is the definition of the
    /// constant/static attribute, or the function implementation. Many times, it is not
    /// possible to add all the modifiers.
    virtual std::string getImplementation()     = 0;

    virtual const Container * getContainer() const
        { return myContainer; }
    virtual Container * getContainer()
        { return myContainer; }

    /// Looks whether the provided visibility is valid
    /// @return NULL if it is not, a pointer to the item if it is.
    /// @see Visibility
    static const std::string * lookForVisibilityKeyword(const std::string &str)
        { return StringMan::buscarEnVector( Visibility, str ); }

    /// Looks whether the provided modifier is valid
    /// @return NULL if it is not, a pointer to the item if it is.
    /// @see Modifier
    static const std::string * lookForModifierKeyword(const std::string &str)
        { return StringMan::buscarEnVector( Modifier, str ); }

    /// Looks whether the provided storage is valid
    /// @return NULL if it is not, a pointer to the item if it is.
    /// @see Storage
    static const std::string * lookForStorageKeyword(const std::string &str)
        { return StringMan::buscarEnVector( Storage, str ); }

    /// Looks whether the provided type is valid
    /// @return NULL if it is not, a pointer to the item if it is.
    /// @see Type
    static const std::string * lookForTypeKeyword(const std::string &str)
        { return StringMan::buscarEnVector( Type, str ); }

    /// Returns the source code line in which this member was defined
    unsigned int getLineNumber() const
        { return lineNumber; }

protected:
    /// Follows all containers of a member until the unique identifier is built
    std::string buildQualifiedName(Container *);
private:
    /// Bears tge type of the member, provided it is native
    const std::string * type;

    /// Bears tge type of the member, provided it is user-defined
    std::string userDefinedType;

    /// Bears the storage type for this member
    const std::string * storage;

    /// Bears the set of modifiers for this member
    /// @see Modifiers
    Modifiers modifiers;

    /// The container this member pertains to
    /// @see Container
    Container * myContainer;

    /// Bears the visibility of this member
    const std::string * visibility;

    bool pointer;
    bool reference;
    unsigned int lineNumber;
};


class Namespace;
class Attribute;
class Method;

/// Classes are a kind of containers, that contain
/// members of type Method and Attribute
/// @see Member, Method, Attribute
class Class : public Container {
friend class Namespace;
public:
    static const std::string Colon;

    /// Holds the information for a given parent class
    class Parent {
    public:
        /// Creates a new Parent class for inheritance
        /// @param n The name of the parent class
        /// @param v The visibility of the parent class
        Parent(const std::string & n,const std::string * v);

        /// Returns the parent name
        const std::string &getParentName() const
            { return parentName; }

        /// Returns the visibility of the parent class in inheritance
        /// @return A std::string holding the visibility
        const std::string &getInheritanceVisibility() const
            { return *inheritanceVisibility; }

        /// Returns the visibility of the parent class in inheritance
        /// @return A pointer to std::string holding the visibility
        /// @see Member::Visibility
        const std::string * getSystemInheritanceVisibility() const
            { return inheritanceVisibility; }

        /// Returns a description of this parent class
        /// @return A std::string with inheritance visibility, space, and parent class name
        std::string toString() const
            { return ( getInheritanceVisibility() + ' ' + getParentName() ); }

    private:
        std::string parentName;
        const std::string * inheritanceVisibility;
    };

    /// The type for storing information about inheritance for this class
    /// @see Parent
    typedef std::vector<Parent> ParentList;

private:
    /// Holds the information about inheritance for this class
    /// @see ParentList
    ParentList parentList;

    /// A list of attributes
    /// @see Member, Container::MembersList
    MembersList attributes;

    /// A list of attributes
    /// @see Member, Container::MembersList
    MembersList methods;

    /// Visibility of this class
    /// @see Member::Visibility
    const std::string * visibility;
public:
    Class(Container * c, Module *m, const std::string &n = "")
        : Container( c, m, n ), visibility( &Member::PublicVisibility )
        { setCurrentVisibility( &Member::PrivateVisibility ); }
    ~Class();

    /// Returns the name of the class parent in inheritance
    /// @return a std::string containing the name. If empty, no inheritance at all.
    /// @see parentName
    const ParentList &getParentList() const
        { return parentList; }

    /// Modifies the parent list of inheritance classes
    /// @param pl A ParentList vector
    /// @see ParentList
    void setParentList(const ParentList &pl)
        { parentList.clear(); parentList = pl; }

    const std::string * getClassVisibility()
        { return visibility; }
    void setClassVisibility(const std::string * v);

    /// Returns the declaration of the header of the class (class name and inheritance)
    /// @return the declaration of the class as a std::string
    std::string getDeclaration();

    /// Adds an attribute to the list of attributes for this class
    /// @param atr An object of the Attribute class
    /// @see Container::MembersList, attributes, Attribute
    Attribute &addAttribute(Attribute & atr);

    /// Adds a method to the list of methods for this class
    /// @param atr An object of the Method class
    /// @see Container::MembersList, methods, Method
    Method &addMethod(Method & atr);

    /// Adds an inner class to this class
    /// @param cln The name of the inner class
    /// @return A pointer to the new created Class object
    Class * addClass(const std::string &cln);

    void chkBasic()  const;
    void chkHigh()   const;
    void chkMedium() const;
    void chkLow()    const;
};

/// Abstract class for attributes (in classes) and constants (in namespaces)
class Data : public Member {
public:
    Data(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Member( l, n, t )
        {}
    Data(unsigned int l,const std::string &n, const std::string &t)
        : Member( l, n, t )
        {}

    /// Sets the initialization value for this data member
    /// This is only possible if the member is static
    /// @param v A std::string containing the intialization literal
    virtual void setInitialValue(const std::string &v)
        { initValue = v; }

    /// Gets the initiatization value for this data member
    /// @return The literal for initialization
    virtual const std::string &getInitialValue() const
        { return initValue; }
private:
    std::string initValue;
};

/// Parent class with members related to classes
class ClassRelated {
public:
    /// Returns the name of the member with full qualification (A::B::C...)
    virtual std::string getQualifiedName() = 0;

    /// Checkings specifically related to members of classes
    static void chk(const Member &);
};

/// Class representing attributes of a class
class Attribute : public Data, public ClassRelated {
public:
    /// Constructor for attributes of primitive data type
    /// @param l The line number where this attribute was found
    /// @param n The name of the attribute
    /// @param t The primitive type (defaults to void)
    /// @see Member::Types
    Attribute(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Data( l, n, t )
        {}

    /// Constructor for attributes of user-defined data type
    /// @param l The line number where this attribute was found
    /// @param n The name of the attribute
    /// @param t The user-defined type, as std::string
    Attribute(unsigned int l, const std::string &n, const std::string &t)
        : Data( l, n, t )
        {}

    void chkBasic()  const;
    void chkHigh()   const;
    void chkMedium() const;
    void chkLow()    const;

    /// When it is static, it can hold an inital value
    /// @param v The initial value
    void setInitialValue(const std::string &v);

    /// The prototype of the attribute (in the header file)
    std::string getPrototype();

    /// The prototype of the attribute (in the implementation file)
    std::string getImplementation();

    /// The attribute when it goes online
    std::string getInline()
        { return std::string(); }

    const Class * getContainer() const
        { return (Class *) Member::getContainer(); }
    Class * getContainer()
        { return (Class *) Member::getContainer(); }

    std::string getQualifiedName()
        { return buildQualifiedName( this->getContainer() ); }
};

/// Class representing methods and functions
class Code : public Member {
public:
    /// Constructor for code returning a primitive data type
    /// @param l The line number in which it was found
    /// @param n The name of the function
    /// @param t The primitive return type (defaults to void)
    /// @see Member::Types
    Code(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Member( l, n, t )
    {}

    /// Constructor for code returning am user-defined data type
    /// @param l The line number in which it was found
    /// @param n The name of the function
    /// @param t The user-defined return type (as std::string)
    Code(unsigned int l, const std::string &n, const std::string &t)
        : Member( l, n, t )
    {}

    /// Returns the set of instructions for this function (no {})
    /// @return The whole set of instructions as std::string
    virtual const std::string getBody() const
        { return body; }

    /// Returns the set of parameters for this function (no ())
    /// No individual parsing is done
    /// @return The whole set of parameters as std::string
    virtual const std::string getParameters() const
        { return parameters; }

    /// Changes the set of instructions for this function
    /// @param b A std::string with the new body
    virtual void setBody(const std::string &b)
        { body = b; }

    /// Changes the set of parameters for this function
    /// @param b A std::string with the new parameters
    virtual void setParameters(const std::string &p)
        { parameters = p; }

    /// Determines whether the function is inline or not (helper function)
    /// @return true if it has InlineStorage, false otherwise
    /// @see Member::Storages
    bool isInline() const
        { return getSystemStorage() == &Member::InlineStorage; }
private:
    std::string body;
    std::string parameters;
};

/// Represents a method (a member function whithin a class)
class Method : public Code, public ClassRelated {
public:
    static const std::string OpenBrace;

    /// Constructor for methods returning a primitive data type
    /// @param l The line number in which it was found
    /// @param n The name of the method
    /// @param t The primitive return type (defaults to void)
    /// @see Member::Types
    Method(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Code( l, n, t ), constFunction( false ), pureVirtualFunction( false )
    {}

    /// Constructor for methods returning an user-defined data type
    /// @param l The line number in which it was found
    /// @param n The name of the method
    /// @param t The user-defined return type (as an std::string)
    Method(unsigned int l, const std::string &n, const std::string &t)
        : Code( l, n, t ), constFunction( false ), pureVirtualFunction( false )
    {}

    std::string getPrototype();
    std::string getImplementation();
    std::string getInline();

    /// Changes whether the method is const (must have a const on its right)
    /// @param v Whether it is or not (defaults to true)
    void setConstMethod(bool v = true)
        { constFunction = v; }

    /// Determines whether the method is const (must have a const on its right)
    /// @return Whether it is or not (true, false)
    bool isConstMethod()
        { return constFunction; }

    /// Returns the quicklist initialisation for the constructor (no individual parsing is done)
    /// @return The quickinitlist as std::string
    const std::string getQuickInitList() const
        { return quickInit; }

    /// Determines the quicklist initialisation for the constructor (no individual parsing is done)
    /// @param q The quickinitlist as std::string
    void setQuickInitList(const std::string &q)
        { quickInit = q; }

    /// Determines whether the method is a constructor
    /// @return true if it is, false otherwise
    bool isConstructor() const;

    /// Determines whether the method is a destructor
    /// @return true if it is, false otherwise
    bool isDestructor() const;

    /// Determines whether the method is pure and virtual
    /// @return true if it is, false otherwise
    bool isPureVirtual() const;

    /// Determines whether the method is virtual
    /// @return true if it is, false otherwise
    bool isVirtual() const
        { return Member::lookForModifier( Member::MdfVirtual ) != NULL; }

    void chkBasic()  const;
    void chkLow()    const;
    void chkMedium() const;
    void chkHigh()   const;

    /// Sets that the method is pure and virtual
    void setPureVirtual();

    /// Sets the body of the method
    /// @param b The body as std::string
    void setBody(const std::string &b);

    const Class * getContainer() const
        { return (Class *) Member::getContainer(); }
    Class * getContainer()
        { return (Class *) Member::getContainer(); }
    std::string getQualifiedName()
        { return buildQualifiedName( this->getContainer() ); }

    /// Reads the parameters of the method, eliminating the initializations
    static void processParameters(std::string &parameters);
private:
    bool constFunction;
    std::string quickInit;
    bool pureVirtualFunction;
};

class Function;
class Constant;

/// Class representing namespaces
class Namespace : public Container {
friend class Module;
private:
    /// A list of constants
    /// @see Member::MembersList
    MembersList constants;

    /// A list of functions
    /// @see Member::MembersList
    MembersList functions;

    /// The current class inside this namespace (if any)
    Class * currentClass;

    /// Total number of classes in this namespace
    unsigned int numberOfClasses;
public:
    /// Constructor for namespaces
    /// @param c Outer (parent) namespace
    /// @param m Module this namespace belongs to
    /// @param ns Name of the namespace
    Namespace(Namespace * c, Module *m, const std::string &ns = "")
        : Container( c, m, ns ), currentClass(NULL), numberOfClasses(0)
        { setCurrentVisibility( &Member::PublicVisibility ); }

    ~Namespace();

    /// The current class inside this namespace (if any)
    /// @return the current class inside this namespace
    Class * getCurrentClass()
        { return currentClass; }

    Namespace * getContainer()
        { return (Namespace *) Container::getContainer(); }

    /// Sets the visibility at the current parsing point in the namespace
    /// @param v A pointer to std::string
    /// @see Member::Visibility
    void setCurrentVisibility(const std::string *v)
        {
            if ( v == &Member::ProtectedVisibility )
                 throw SemanticError( "protected visibility not allowed here" );
            else Container::setCurrentVisibility( v );
        }

    /// Add a new Constant to the constants list
    /// @param c The new Constant object to add to the list
    /// @see Constant
    Constant &addConstant(Constant &c);

    /// Add a new Function to the constants list
    /// @param c The new Function object to add to the list
    /// @see Function
    Function &addFunction(Function &f);

    /// Get the constants list
    /// @return the constants list, as a MemberList object
    /// @see Member::MemberList
    MembersList &getConstants()
        { return constants; }

    /// Get the functions list
    /// @return the functions list, as a MemberList object
    /// @see Member::MemberList
    MembersList &getFunctions()
        { return functions; }

    /// Get the constants list
    /// @return the constants list, as a MemberList object
    /// @see Member::MemberList
    const MembersList &getConstants() const
        { return constants; }

    /// Get the functions list
    /// @return the functions list, as a MemberList object
    /// @see Member::MemberList
    const MembersList &getFunctions() const
        { return functions; }

    /// Creates a new namespace
    /// @param nsName the namespace name
    Namespace * addNamespace(const std::string &nsName);

    /// Creates a new class
    /// @param n the class name
    Class * addClass(const std::string &n);

    /// Returns the current namespace
    /// @return A pointer to a Namespace  object
    Namespace * getCurrentNamespace()
        { return (Namespace *) getCurrentContainer(); }

    /// Returns the current namespace
    /// @return A pointer to a Namespace  object
    const Namespace * getCurrentNamespace() const
        { return ( const Namespace *) getCurrentContainer(); }

    /// Go up a level in container nesting namespace > namespace > class...
    /// @return A pointer to the Container which is going to be the current one
    Container * setOuterAsCurrentContainer();

    /// Determine whether this namespace has classes inside it or not
    bool hasClass() const
        { return ( numberOfClasses > 0 ); }

    /// Determine whether this namespace has functions inside it or not
    bool hasFunctions() const
        { return !( getFunctions().empty() ); }

    /// Determine whether this namespace has constants inside it or not
    bool hasConstants() const
        { return !( getConstants().empty() ); }

    /// Determine whether this namespace has something inside it
    bool isEmpty() const
        { return ( !hasConstants()
                && !hasFunctions()
                && !hasClass() );
        }

    /// Determine whether this namespace is inside another one or not
    bool isInnerNamespace() const;

    void chkBasic()   const;
    void chkLow()     const;
    void chkMedium()  const;
    void chkHigh()    const;
};

class EntryPoint;

/// The class representing modules
class Module: public Entity {
public:
    /// A type for storing a list of dependencies
    typedef std::vector<std::string> Dependencies;
    /// A type for pointing to a member of the dependencies list
    typedef Dependencies::iterator PtrDependencies;

    /// The mark (prefix) identifying directives (#include, #ifdef...)
    static const std::string DirectiveMark;

    /// Double-quotes mark
    static const std::string DoubleQuote;

    // Reserved words
    static const std::string RWordInclude;
    static const std::string RWordImport;
    static const std::string RWordExtern;
    static const std::string RWordUsing;
    static const std::string RWordClass;
    static const std::string RWordNamespace;
    static const std::string RWordTypedef;

    /// The prefix for the header guard constant
    static const std::string ModulePrefix;

    /// A string containing all C++ standard headers
    static const std::string StandardHeaders;

    /// This identifies the finite-automata stages
    enum State { TopLevel, NamespaceLevel, ClassLevel };
private:
    State state;
    Dependencies dependencies;
    std::auto_ptr<EntryPoint> entryPoint;
    Namespace * mainNamespace;
    Namespace * currentNamespace;
public:
    /// Constructor for modules
    /// @param ns The name of the module
    Module(const std::string &ns = "")
        : Entity( ns ), state( TopLevel ), mainNamespace(NULL), currentNamespace( NULL )
        {}
    virtual ~Module();

    /// Change the name of the module
    void setName(const std::string &name);

    /// The entry point is the main function
    /// @param f A pointer to the EntryPoint object holding the main function
    void setEntryPoint(EntryPoint *f)
        { entryPoint.reset( f ); }

    /// The entry point is the main function
    /// @param f A pointer to the EntryPoint object holding the main function (NULL if not set)
    EntryPoint * getEntryPoint() const
        { return entryPoint.get(); }

    /// Returns the State of the finite-automata
    /// @return the state as a State object
    /// @see State
    State getState() const
        { return state; }

    /// Changes the state or simply resets it to TopLevel (default)
    /// @param st A State object
    /// @see State
    void resetState(State st = TopLevel )
        { state = st; }

    Namespace * getCurrentNamespace()
        { return currentNamespace; }

    const Namespace * getCurrentNamespace() const
        { return currentNamespace; }

    /// Adds a new namespace in this module, at top-level
    /// @param name The name of the new namespace
    /// @return A pointer to the newly created Namespace object
    Namespace * addNamespace(const std::string &name);

    /// Returns the top level namespace (or the first of them, if more than one)
    /// @return A pointer to the Namespace object
    Namespace * getMainNamespace()
        { return mainNamespace; }

    /// Returns the top level namespace (or the first of them, if more than one)
    /// @return A pointer to the Namespace object
    const Namespace * getMainNamespace() const
        { return mainNamespace; }

    /// Go up a level in container nesting namespace > namespace...
    /// @return A pointer to the Namespace which is going to be the current one
    Namespace * setOuterAsCurrentNamespace();

    /// Adds a dependency, namely the included (#include) modules
    /// @param d The new module
    void addDependency(const std::string &d);

    /// Returns a list of dependencies for this module
    /// @return A Dependencies object
    const Dependencies &getDependencies() const
        { return dependencies; }

    void chkBasic()  const;
    void chkHigh()   const;
    void chkMedium() const;
    void chkLow()    const;

    /// Helper function that checks a given namespace
    /// @param ns A pointer to the Namespace object to check
    void chkNamespace(const Namespace * ns) const;
};

/// This class represents Data and Code which is going to be
/// inside a namespace: Function and Constant
class NamespaceRelated {
public:
    /// Returns a fully-qualified name in the form of A::B::C
    virtual std::string getQualifiedName() = 0;

    /// Helper function: checks a given member inside a Namespace
    static void chk(const Member &);
};

/// Class representing functions in the code (no member functions)
class Function : public Code, public NamespaceRelated {
public:
    /// Function constructor
    /// @param l The line number in which this function appears
    /// @param n The name of the function
    /// @param t The return type of the function, provided it is a primitive data type
    Function(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Code( l, n, t )
    {}
    /// Function constructor
    /// @param l The line number in which this function appears
    /// @param n The name of the function
    /// @param t The return type of the function, provided it is a user-defined data type
    Function(unsigned int l, const std::string &n, const std::string &t)
        : Code( l, n, t )
    {}

    std::string getPrototype();
    std::string getImplementation();
    std::string getInline();

    const Namespace * getContainer() const
        { return (Namespace *) Member::getContainer(); }
    Namespace * getContainer()
        { return (Namespace *) Member::getContainer(); }
    std::string getQualifiedName()
        { return buildQualifiedName( this->getContainer() ); }

    void chkBasic()  const;
    void chkLow()    const;
    void chkMedium() const;
    void chkHigh()   const;
};

class Constant : public Data, public NamespaceRelated {
public:
    /// Constant constructor
    /// @param l The line number in which this Constant appears
    /// @param n The name of the Constant
    /// @param t The return type of the Constant, provided it is a primitive data type
    Constant(unsigned int l, const std::string &n, const std::string * t = &VoidType)
        : Data( l, n, t )
        {}
    /// Constant constructor
    /// @param l The line number in which this Constant appears
    /// @param n The name of the Constant
    /// @param t The return type of the function, provided it is a user-defined data type
    Constant(unsigned int l, const std::string &n, const std::string &t)
        : Data( l, n, t )
        {}

    std::string getPrototype();
    std::string getImplementation();

    std::string getInline()
        { return std::string(); }

    const Namespace * getContainer() const
        { return (Namespace *) Member::getContainer(); }
    Namespace * getContainer()
        { return (Namespace *) Member::getContainer(); }

    std::string getQualifiedName()
        { return buildQualifiedName( this->getContainer() ); }

    void chkBasic()  const;
    void chkLow()    const;
    void chkMedium() const;
    void chkHigh()   const;
};

/**
    This is a wrapper function for a specific use of a Function class: the main() function.
    It'd not make sense to inherit from Function, since EntryPoint is not
    of class Member.
    @see Entity, Member, Function
*/
class EntryPoint : public Entity {
public:
    /** Constructor. The only needed parameter is the line number,
        since we already know the remainig data: (function name... etc.)
        @param l The line number of the source file in which it is found.
    */
    EntryPoint(unsigned int l);

    /// Returns the implementation (for the cpp file) of this entity
    /// The getPrototype() function is not needed, since there is no prototype for main()
    /// @return The entrypoint source code, ready to be written to the cpp file, as an std::string
    std::string getImplementation();

    /// Returns the source code corresponding to the instructions inside the EntryPoint
    /// @return The source code, as std::string
    /// @see Function, Function::getBody
    const std::string getBody() const
        { return fn->getBody(); }

    /// Returns the parameters corresponding to the entrypoint
    /// @return The parameters, as std::string
    /// @see Function, Function::getParameters
    const std::string getParameters() const
        { return fn->getParameters(); }

    /// Stores the body of main()
    /// @param b A std::string containing the body of main()
    /// @see Function, Function::setBody
    void setBody(const std::string &b)
        { fn->setBody( b ); }

    /// Stores the parameters of main()
    /// @param b A std::string containing the parameteres of main()
    /// @see Function, Function::setParameters
    void setParameters(const std::string &p)
        { fn->setParameters( p ); }

    /// Returns the line number in which main() was found in the source code
    /// @return The line number
    unsigned int getLineNumber() const
        { return fn->getLineNumber(); }

    void chkBasic()  const;
    void chkLow()    const;
    void chkMedium() const;
    void chkHigh()   const;
private:
    std::auto_ptr<Function> fn;
};

}

}

#endif // CP3TDS_H_INCLUDED
