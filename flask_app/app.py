from flask import Flask, render_template, jsonify, request
import csv
import os
from collections import deque, defaultdict

app = Flask(__name__)

# In-memory Datastructures to simulate C++ backend
movies_list = []
movies_dict = {} 
graph = defaultdict(set)
node_types = {} 

def load_data():
    csv_path = os.path.join(os.path.dirname(__file__), '..', 'movie_metadata.csv')
    if not os.path.exists(csv_path):
        print(f"Dataset not found at {csv_path}")
        return

    try:
        with open(csv_path, 'r', encoding='utf-8') as f:
            reader = csv.DictReader(f)
            for row in reader:
                title = row['movie_title'].strip().replace('', '').replace('\xa0', '')
                if not title: continue
                
                actors = [a.strip() for a in [row.get('actor_1_name'), row.get('actor_2_name'), row.get('actor_3_name')] if a and a.strip()]
                director = row.get('director_name', '').strip()
                genres = [g.strip() for g in row.get('genres', '').split('|') if g.strip()]
                
                movie_data = {
                    "title": title,
                    "director": director,
                    "actors": actors,
                    "genres": genres,
                    "year": row.get('title_year', 'N/A') or 'N/A',
                    "rating": row.get('imdb_score', 'N/A') or 'N/A'
                }
                
                movies_list.append(movie_data)
                movies_dict[title.lower()] = movie_data
                node_types[title] = 'movie'
                
                # Build undirected Graph edges
                if director:
                    graph[title].add(director)
                    graph[director].add(title)
                    node_types[director] = 'director'
                
                for actor in actors:
                    graph[title].add(actor)
                    graph[actor].add(title)
                    node_types[actor] = 'actor'
                    
                for genre in genres:
                    graph[title].add(genre)
                    graph[genre].add(title)
                    node_types[genre] = 'genre'
                    
        # --- ADD TENET MANUALLY FOR DEMONSTRATION ---
        tenet_title = "Tenet"
        tenet_data = {
            "title": tenet_title,
            "director": "Christopher Nolan",
            "actors": ["John David Washington", "Robert Pattinson", "Elizabeth Debicki"],
            "genres": ["Action", "Sci-Fi", "Thriller"],
            "year": "2020",
            "rating": "7.3"
        }
        movies_list.append(tenet_data)
        movies_dict[tenet_title.lower()] = tenet_data
        node_types[tenet_title] = 'movie'
        
        graph[tenet_title].add("Christopher Nolan")
        graph["Christopher Nolan"].add(tenet_title)
        node_types["Christopher Nolan"] = 'director'
        
        for g in tenet_data["genres"]:
            graph[tenet_title].add(g)
            graph[g].add(tenet_title)
            node_types[g] = 'genre'
            
        for a in tenet_data["actors"]:
            graph[tenet_title].add(a)
            graph[a].add(tenet_title)
            node_types[a] = 'actor'
        # --------------------------------------------
                    
    except Exception as e:
        print(f"Error loading CSV: {e}")

# Load dataset on startup
load_data()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/stats')
def get_stats():
    return jsonify({
        "total_nodes": f"{len(graph):,}",
        "total_edges": f"{sum(len(v) for v in graph.values()) // 2:,}",
        "index_type": "Distributed AVL/Hash Map",
        "status": "Optimal"
    })

@app.route('/api/suggest')
def suggest():
    query = request.args.get('q', '').lower()
    if len(query) < 2:
        return jsonify([])
    
    results = []
    count = 0
    # O(N) search for suggestion mock
    for node in graph.keys():
        if query in node.lower():
            results.append({"name": node, "type": node_types.get(node, 'unknown')})
            count += 1
            if count >= 8: break
            
    return jsonify(results)

@app.route('/api/search')
def search():
    query = request.args.get('q', '').lower()
    
    if not query:
        return jsonify(movies_list[:12]) # Default view
        
    results = []
    for m in movies_list:
        # Case insensitive multi-attribute match
        if (query in m['title'].lower() or 
            query in m['director'].lower() or 
            any(query in a.lower() for a in m['actors']) or 
            any(query in g.lower() for g in m['genres'])):
            
            results.append(m)
            if len(results) >= 20: # Cap to 20 results for performance
                break
                
    return jsonify(results)

@app.route('/api/recommend')
def recommend():
    movie_query = request.args.get('movie', '').lower()
    
    # Resolve node
    target_movie = None
    for m in movies_dict.keys():
        if movie_query in m:
            target_movie = movies_dict[m]['title']
            break
            
    if not target_movie:
        return jsonify({"error": "Movie not found in dataset."})

    # BFS Traversal logic for recommendations
    scores = defaultdict(int)
    shared = defaultdict(list)
    
    for neighbor in graph[target_movie]:
        n_type = node_types.get(neighbor)
        if n_type in ['actor', 'director', 'genre']:
            for other_movie in graph[neighbor]:
                if other_movie != target_movie and node_types.get(other_movie) == 'movie':
                    scores[other_movie] += 1
                    shared[other_movie].append(f"{n_type.capitalize()}: {neighbor}")
                    
    # Sort and top 5
    sorted_recs = sorted(scores.items(), key=lambda x: x[1], reverse=True)[:5]
    
    recommendations = []
    max_score = len(graph[target_movie]) if target_movie in graph else 1
    
    for m_title, score in sorted_recs:
        perc = min(99, int((score / max(1, max_score)) * 100 + 35))
        recommendations.append({
            "title": m_title,
            "match_score": f"{perc}%",
            "shared_attributes": shared[m_title][:3] # Show max 3 attributes
        })
        
    return jsonify({"target": target_movie, "recs": recommendations})

@app.route('/api/path')
def find_path():
    start_q = request.args.get('start', '').lower()
    end_q = request.args.get('end', '').lower()
    
    start_node = next((n for n in graph.keys() if start_q == n.lower()), None)
    if not start_node:
        start_node = next((n for n in graph.keys() if start_q in n.lower()), None)
        
    end_node = next((n for n in graph.keys() if end_q == n.lower()), None)
    if not end_node:
        end_node = next((n for n in graph.keys() if end_q in n.lower()), None)
    
    if not start_node or not end_node:
        return jsonify({"error": f"Nodes not found in dataset."})
        
    # BFS
    queue = deque([(start_node, [start_node])])
    visited = set([start_node])
    
    while queue:
        curr, path = queue.popleft()
        
        if curr == end_node:
            result_path = [{"node": n, "type": node_types.get(n, "unknown")} for n in path]
            return jsonify({"path": result_path, "hops": len(path)-1})
            
        for neighbor in graph[curr]:
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append((neighbor, path + [neighbor]))
                
    return jsonify({"error": "No path found connecting these entities."})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
