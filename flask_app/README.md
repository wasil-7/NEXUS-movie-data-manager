# NEXUS: Graph-Based Movie Data Manager

NEXUS is an enterprise-grade, distributed-system-inspired frontend visualization for the Movies Data Manager. Designed to showcase advanced data structures (AVL Trees, Hash Maps, and Graphs) with an incredibly polished, scalable, and responsive interface.

## 🌟 Features

- **O(log N) Indexed Search**: Fast, responsive queries simulating AVL/BST tree and Hash Map lookups.
- **Traversal Recommendations**: Suggests connected nodes using BFS/DFS traversal scoring.
- **Shortest Path Discovery**: Beautifully visualizes the exact path and degrees of separation between any two entities in the graph network.
- **Premium Glassmorphic UI**: High-end aesthetic with dynamic node-edge background simulations and real-time scaling statistics.

## 📸 Interface Preview

### Dashboard & Indexed Search
![Dataset Search](assets/1.png)
*Demonstration of the indexed search capabilities and global cluster status.*

### Graph Traversal & Shortest Path
![Graph Traversal](assets/2.png)
*Visualizing BFS/DFS graph traversals and entity relationships.*

## 🚀 Running the App Locally

This is a lightweight Python Flask application.

1. Ensure Flask is installed:
   ```bash
   python -m pip install flask
   ```
2. Start the local server:
   ```bash
   python app.py
   ```
3. Visit `http://127.0.0.1:5000` in your web browser.
