# kiyosGameEngine
kiyoGameEngine (kge) is a small header only C++17 library, that provides boilerplate code for a small to medium sized games. Currently kge includes a main game loop with a state stack, a simple point class and an entity component system. Each header is independent and only requires the STL and C++17 compatible compiler. It is designed to be used with your favourite graphics/sound library for your OS.

# Install
Just include the kge header files in your project to include kge. Examples and tests built on Ubuntu 18.04.1 LTS with gcc. The example requires SFML.

# To use 
The best way to see how kge works is to view the example and perhaps the tests.

 
The kgePointLine header is a simple implementation of a 2D point class with overloaded operators that act on two fundamental types 'x' and 'y'. Also, a line class that has two points as members is inlcuded that has a useful intersect method. The type of x and y for both point and line is defined by the template parameter.

 
The kgeThreadPool is a simple thread pool implementation. Avoid using multiple threads when creating or deleting entities. Some of the static members cause even modification of distinct objects to be unsafe. None of the objects so far are safe for shared modification. 

 
The kgeMainLoop header holds a main game loop and a state stack. Create a number of states that inherit from kge::abstractState and write implementations for the update(),render(), and processEvents() virtual functions. Both update and render are called with “double dt” which provides a double with a change in time between the loops in seconds. The kgeMainLoop class handles changing state, by switching/pushing/poping and “double popping” states (e.g. a pause dialog state with a quit option may be pushed on top of a game state; to quit the game, both states will need to be pushed). These are implemented as class method templates where the state and all its constructor argument types are passed as parameters. Arguments to the call are passed by value and can hold pointers to resources or data and any other important singleton objects such as an OS window instance.

 
The kgeECS header includes a kge::baseEntity class that the game entities should inherit from. This will give each entity a unique ID member. All entities and any potential components (POD types) should be held in kge::entCompHandler class template, which is simply a vector container of unique_ptr with some helper methods. The fundamental idea is that the entity ID is also the index of the entities and components in the kge::entCompHandler container… so we can get constant time access to the entity and its components from just the ID. Before dereferencing anything from a kge::entCompHandler, it is good practice to first check to ensure the entities weren't deleted by checking for a nullptr at that entity ID's index, then do the same for components to avoid segmentation faults. If you don't want to loop through all the entities a good trick is to create a static unordered_set of IDs somewhere and insert and erase IDs within the  constructor/destructor of whatever baseEntity derived class (see renderSystem in example). The system class simply requires the update() method to be implemented. A clean-up system exists that should take all the kge::entCompHandles as lvalue references and ties them together in a std::tuple. The update method calls a clean-up method on all the tge::entCompHandler items that were passed to the constructor that recycles the entity IDs for reuse and deletes the entities components. 

 
…yeah just look at the example; kge is surprisingly small and the templates are digestible, so it shouldn't be too difficult to work out what's going on. 

