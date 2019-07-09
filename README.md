# Ushiro

*ushiro* is an experimental, but battle proven, small library supporting the "backend" of a typical UI application.
It helps to implement a unidirectional UI architecture similar to redux.
 * Experimental means that the interface is not stable! Use with caution
 * Battle-proven means that it (or the "predecessor" code) is actively used in a couple of production projects

# Design

*ushiro* uses a generic event_bus to implement the forward "action path", i.e. reacting to user or remote input.
For the backward "store to view" path, it uses a lightweight "diffing" approach that makes it easy to read and react to changes in the data model in the same way.
It is usually used together with a Qt based UI.

# Interesting links

* [lager](https://github.com/arximboldi/lager) is a great unidirectional UI library for C++
* [redux](https://redux.js.org/) is probably the most famous unidrectional UI library, but for JavaScript
