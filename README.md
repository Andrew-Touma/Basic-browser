# Browser History Management System

A C++ application designed to manage web browsing history and bookmarks, implementing several fundamental data structures and algorithms to provide efficient history tracking and retrieval.

##  Features

### Core Functionalities
- **File-Based Data Persistence**: Parses input files to load history and bookmarks, and writes the current state back to a file for persistence.
- **History Management**: Create, delete, and search for web pages in the browsing history.
- **Bookmark System**: Add pages to bookmarks, mark them as favorites, and track visit counts.
- **Chronological View**: Displays browsing history sorted by the most recently visited pages.
- **Related Pages Tracking**: Each page can maintain a list of related web pages.

###  Data Structures & Algorithms

This project focuses on the practical implementation of linked structures and basic algorithms:

#### 1. Doubly Linked Lists
The core of the system is built on doubly linked lists to allow efficient bidirectional traversal and deletions:
- **`pages_History`**: A doubly linked list of `Page` nodes. This allows the system to easily add new pages to the tail and remove pages from any position.
- **`bookMarkList`**: A doubly linked list of `bookMarkNode` objects, facilitating the management of bookmarked pages.

#### 2. Singly Linked Lists
- **`related_Pages`**: Each `Page` node contains a pointer to a singly linked list of related pages. This represents a one-to-many relationship between a primary page and its related links.

#### 3. Algorithms
- **Sorting**: Implements a bubble sort variant in `displayHistory()` to order pages by their `visitedAt` timestamp (Descending order).
- **Searching**: 
    - **Substring Search**: Performs a linear search through the history to find URLs containing a specific substring.
    - **ID-Based Lookup**: Efficiently locates specific pages using their unique `P_ID`.
- **Selection/Optimization**:
    - **Most Related Page**: Iterates through history to find the page with the maximum number of related links.
    - **Most Visited Page**: Cross-references the history with bookmarks to find the page with the highest visit count.
    - **Recent Retrieval**: An algorithm to extract the top `N` most recently visited bookmarks.
- **Cleanup**: Implements a filter-and-delete algorithm to remove all pages older than a specific cutoff date, ensuring referential integrity by also removing associated bookmarks.

##  Technical Specifications

- **Language**: C++
- **Key Structures**:
    - `Page`: Stores ID, URL, Title, and Visit Time.
    - `bookMarkNode`: Stores a reference to a `Page`, favorite status, visit count, and last visited time.
    - `related_Pages`: Stores related page metadata.

##  How to Use

1. **Run the Application**: Compile and run the `P1.exe`.
2. **Load Data**: Use option `1` to parse an input file (following the `-` for pages, `*` for related pages, and `#` for bookmarks format).
3. **Manage History**: Use the menu options to create pages, delete old history, or search by URL.
4. **Manage Bookmarks**: Add pages to bookmarks and retrieve the most recent ones.
5. **Save Progress**: Use option `9` to write the current state to an output file.
