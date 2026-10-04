// DOM Elements
const navItems = document.querySelectorAll('.nav-item');
const viewSections = document.querySelectorAll('.view-section');
const searchResults = document.getElementById('search-results');
const recommendResults = document.getElementById('recommend-results');
const pathResults = document.getElementById('path-results');

// Initial Load
document.addEventListener('DOMContentLoaded', () => {
    fetchStats();
    performSearch(''); // Load initial data
});

// Navigation Logic
navItems.forEach(item => {
    item.addEventListener('click', () => {
        navItems.forEach(n => n.classList.remove('active'));
        viewSections.forEach(v => v.classList.remove('active'));
        item.classList.add('active');
        const targetId = item.getAttribute('data-target');
        document.getElementById(targetId).classList.add('active');
    });
});

// Fetch Stats
async function fetchStats() {
    try {
        const response = await fetch('/api/stats');
        const data = await response.json();
        document.getElementById('stat-nodes').textContent = data.total_nodes;
        document.getElementById('stat-edges').textContent = data.total_edges;
    } catch (error) {
        console.error('Error fetching stats:', error);
    }
}

// Universal Autocomplete Setup
function setupAutocomplete(inputId, dropdownId, onSelectCallback) {
    const input = document.getElementById(inputId);
    const dropdown = document.getElementById(dropdownId);
    let suggestTimeout;
    
    input.addEventListener('input', (e) => {
        const query = e.target.value;
        clearTimeout(suggestTimeout);
        
        if (query.length < 2) {
            dropdown.style.display = 'none';
            if (onSelectCallback && inputId === 'search-input') onSelectCallback(query);
            return;
        }

        suggestTimeout = setTimeout(async () => {
            try {
                const res = await fetch(`/api/suggest?q=${encodeURIComponent(query)}`);
                const suggestions = await res.json();
                
                if (suggestions.length > 0) {
                    dropdown.innerHTML = suggestions.map(s => {
                        // Escape quotes for html
                        const safeName = s.name.replace(/"/g, '&quot;').replace(/'/g, '&#39;');
                        return `<div class="suggestion-item" data-name="${safeName}">
                            <span>${s.name}</span>
                            <span class="suggestion-type">${s.type}</span>
                        </div>`;
                    }).join('');
                    dropdown.style.display = 'block';
                    
                    // Bind click events to new items
                    dropdown.querySelectorAll('.suggestion-item').forEach(item => {
                        item.addEventListener('click', function() {
                            const name = this.getAttribute('data-name');
                            input.value = name;
                            dropdown.style.display = 'none';
                            if (onSelectCallback && inputId === 'search-input') {
                                onSelectCallback(name);
                            }
                        });
                    });
                } else {
                    dropdown.innerHTML = `<div class="suggestion-item" style="color: var(--color-taupe); cursor: default; justify-content: center;">
                        <span>No matches found</span>
                    </div>`;
                    dropdown.style.display = 'block';
                }
            } catch (e) {
                console.error(e);
            }
            if (onSelectCallback && inputId === 'search-input') onSelectCallback(query);
        }, 200);
    });

    // Hide dropdown if clicked outside
    document.addEventListener('click', (e) => {
        if (!e.target.closest(`#${inputId}`) && !e.target.closest(`#${dropdownId}`)) {
            dropdown.style.display = 'none';
        }
    });
}

// Initialize Autocompletes
setupAutocomplete('search-input', 'search-suggestions', performSearch);
setupAutocomplete('recommend-input', 'recommend-suggestions');
setupAutocomplete('path-start', 'path-start-suggestions');
setupAutocomplete('path-end', 'path-end-suggestions');

// Search
async function performSearch(query) {
    searchResults.innerHTML = '<div class="center-content"><div class="loader"></div></div>';
    try {
        const response = await fetch(`/api/search?q=${encodeURIComponent(query)}`);
        const movies = await response.json();
        
        searchResults.innerHTML = '';
        if (movies.length === 0) {
            searchResults.innerHTML = '<p style="color: var(--color-taupe); padding: 24px;">No records found matching query.</p>';
            return;
        }

        movies.forEach((movie, index) => {
            const card = document.createElement('div');
            card.className = 'movie-card glass-panel';
            card.style.animationDelay = `${index * 0.05}s`;
            
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="movie-title">${movie.title}</h3>
                    <span class="movie-year">${movie.year}</span>
                </div>
                <div class="movie-meta">
                    <div class="meta-item"><i class='bx bx-star'></i> ${movie.rating}</div>
                    <div class="meta-item"><i class='bx bx-movie'></i> ${movie.director}</div>
                </div>
                <div class="movie-tags">
                    ${movie.genres.slice(0, 2).map(g => `<span class="tag">${g}</span>`).join('')}
                    ${movie.actors.slice(0, 2).map(a => `<span class="tag">${a}</span>`).join('')}
                </div>
            `;
            searchResults.appendChild(card);
        });
    } catch (error) {
        console.error('Search failed:', error);
        searchResults.innerHTML = '<p style="color: red;">Error fetching results.</p>';
    }
}

// Recommendation
async function fetchRecommendations() {
    const input = document.getElementById('recommend-input').value;
    if (!input) return;

    recommendResults.innerHTML = '<div class="center-content"><div class="loader"></div></div>';

    try {
        const response = await fetch(`/api/recommend?movie=${encodeURIComponent(input)}`);
        const data = await response.json();
        
        if (data.error) {
            recommendResults.innerHTML = `<p style="color: var(--color-caramel);">${data.error}</p>`;
            return;
        }

        recommendResults.innerHTML = `<h3 style="margin-bottom: 16px; color: var(--color-taupe)">Traversing graph for: <span style="color: var(--color-sand)">${data.target}</span></h3>`;
        
        data.recs.forEach((rec, index) => {
            const card = document.createElement('div');
            card.className = 'rec-card glass-panel';
            card.style.animation = `fadeIn 0.5s ease forwards ${index * 0.1}s`;
            card.style.opacity = '0';
            card.style.marginBottom = '16px';
            
            card.innerHTML = `
                <div class="rec-info">
                    <h3 style="margin-bottom: 8px;">${rec.title}</h3>
                    <div class="rec-reasons" style="display: flex; gap: 12px; font-size: 0.9rem; color: var(--color-taupe);">
                        ${rec.shared_attributes.map(attr => `<span><i class='bx bx-git-commit'></i> ${attr}</span>`).join('')}
                    </div>
                </div>
                <div class="match-score" style="font-size: 1.5rem; font-weight: bold; color: var(--color-caramel);">${rec.match_score}</div>
            `;
            recommendResults.appendChild(card);
        });
    } catch (error) {
        console.error('Recommendation failed:', error);
        recommendResults.innerHTML = '<p style="color: red;">Error processing traversal.</p>';
    }
}

// Path Finding
async function fetchPath() {
    const start = document.getElementById('path-start').value;
    const end = document.getElementById('path-end').value;
    
    if (!start || !end) return;

    pathResults.innerHTML = '<div class="center-content"><div class="loader"></div></div>';

    try {
        const response = await fetch(`/api/path?start=${encodeURIComponent(start)}&end=${encodeURIComponent(end)}`);
        const data = await response.json();
        
        if (data.error) {
            pathResults.innerHTML = `<p style="color: var(--color-caramel);">${data.error}</p>`;
            return;
        }
        
        pathResults.innerHTML = '';
        
        data.path.forEach((node, index) => {
            const nodeEl = document.createElement('div');
            nodeEl.className = 'path-node';
            nodeEl.style.animationDelay = `${index * 0.2}s`;
            
            let icon = 'bx-hash'; 
            if (node.type === 'movie') icon = 'bx-movie-play';
            if (node.type === 'actor' || node.type === 'director') icon = 'bx-user';

            nodeEl.innerHTML = `
                <div class="node-circle"><i class='bx ${icon}'></i></div>
                <div class="node-label">${node.node}</div>
                <div class="node-type">${node.type}</div>
            `;
            
            pathResults.appendChild(nodeEl);

            if (index < data.path.length - 1) {
                const edgeEl = document.createElement('div');
                edgeEl.className = 'path-edge';
                edgeEl.style.animationDelay = `${index * 0.2 + 0.1}s`;
                pathResults.appendChild(edgeEl);
            }
        });
        
    } catch (error) {
        console.error('Path finding failed:', error);
        pathResults.innerHTML = '<p style="color: red;">Error finding path.</p>';
    }
}
