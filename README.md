# kiyosGameEngine
kiyoGameEngine (kge) is a small header only C++17 library, that provides boilerplate code for a small to medium sized games. Currently kge includes a main game loop with a state stack, a simple point class and an entity component system. Each header is independent and only requires the STL and C++17 compatible compiler. It is designed to be used with your favourite graphics/sound library.

# Install
Just include the kge header files in your project to include kge. Examples and tests built on Ubuntu 18.04.1. The example requires SFML.

# To use 
The best way to see how kge works is to view the example and perhaps the tests.

 
The kgePointLine header is a simple implementation of a 2D point class and a "line" class (2 points). overloaded operators should provide the basic point arithmetic needed. The line class has a useful intersect method. The type of x and y for both point and line is defined by the template parameter.

 
The kgeThreadPool is a very simple thread pool implementation. useful to paralellize complex for-loops and async calls if needed.

 
The kgeMainLoop header holds a main game loop and a state stack. Create a number of states that inherit from kge::abstractState and write implementations for the update(),render(), and processEvents() virtual functions. Both update and render are called with “double dt” which provides a the change in time between the loops in seconds. The kgeMainLoop class handles changing state, by switching/pushing/poping states; these are implemented as class method templates where the state and all its constructor argument types are passed as parameters. Arguments to the call must be passed by value and intend to hold pointers to resources or any other important singleton object such as an OS window instance.

 
The entity component system is nice and simple, its flexible and made to help you get started. the kge::entityHandler class is a light wrapper around a vector of kge::entity. Supply a unsigned int at construction to assign the maximum entities that will be available. The componentHandler class template is a light wrapper around a vector of components. The template argument can be any POD type to represent the component (e.g. componentHandler<bodyComponent>, note component needs to be default constructable). Any object with an update method can exist as a system to manipulate the entity and component handlers. The strength of this system is that the entity ID is unique and is also its index in the holding vector inside the component or entity handler. So the getItem(ID) methods can get the relevant components for each entity as long as the entity ID is known. The kge::entity only holds a flag to define if it is alive, an ID and a type. The build(type) method will add a new entity of a certain type and adding components to the entity is left to the user, use the return referency to the entty to add components to that entity (will add it to the index of the entity ID.. Also included is a for_each() and for_each_type() methods to loop over each entity that is alive or alive and matches the type given as an argument. This simple system can get you quite far for a small/medium game. more will be required for larger games with more complexity. Generally for the POD types its good to keep variables that are accessed together in the same component, this keeps things close together and can improve performance. Avoid using pointers as lots of indirection will slow things down.

 
…yeah just look at the example; kge is surprisingly small and the templates are digestible, so it shouldn't be too difficult to work out what's going on. 

