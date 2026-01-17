const API_URL = 'http://localhost:3000/api';
let isReverse = false;

// Initialization
document.addEventListener('DOMContentLoaded', () => {
    loadProducts();

    // Add Product Form Listener
    document.getElementById('add-product-form').addEventListener('submit', async (e) => {
        e.preventDefault();

        const product = {
            id: document.getElementById('prod-id').value,
            name: document.getElementById('prod-name').value,
            category: document.getElementById('prod-category').value,
            price: document.getElementById('prod-price').value,
            stock: document.getElementById('prod-stock').value
        };

        try {
            const res = await fetch(`${API_URL}/products`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(product)
            });
            const result = await res.json();
            if (result.status === 'success') {
                showToast(result.message, 'success');
                e.target.reset();
                if (document.getElementById('dashboard').classList.contains('active')) {
                    loadProducts(isReverse);
                }
            } else {
                showToast(result.message || 'Error adding product', 'error');
            }
        } catch (err) {
            console.error(err);
            showToast('Failed to connect to server', 'error');
        }
    });

    // Update Form Listener
    document.getElementById('edit-product-form').addEventListener('submit', async (e) => {
        e.preventDefault();
        const id = document.getElementById('edit-id').value;
        const price = document.getElementById('edit-price').value;
        const stock = document.getElementById('edit-stock').value;

        try {
            const res = await fetch(`${API_URL}/products/${id}`, {
                method: 'PUT',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ price, stock })
            });
            const result = await res.json();
            if (result.status === 'success') {
                showToast(result.message, 'success');
                closeModal();
                loadProducts(isReverse);
            } else {
                showToast(result.message || 'Update failed', 'error');
            }
        } catch (err) {
            console.error(err);
        }
    });

    // Search Listener
    let searchTimeout;
    document.getElementById('search-input').addEventListener('input', (e) => {
        clearTimeout(searchTimeout);
        searchTimeout = setTimeout(() => {
            const query = e.target.value;
            if (query) searchProducts(query);
            else loadProducts(isReverse);
        }, 300);
    });
});

// Navigation
function showSection(sectionId) {
    document.querySelectorAll('.content-section').forEach(el => el.classList.remove('active'));
    document.querySelectorAll('.nav-item').forEach(el => el.classList.remove('active'));

    document.getElementById(sectionId).classList.add('active');

    // Highlight sidebar
    let navIndex = 0;
    if (sectionId === 'add-product') navIndex = 1;
    if (sectionId === 'blockchain') navIndex = 2;
    document.querySelectorAll('.nav-item')[navIndex].classList.add('active');

    // Update Title & Context
    const titles = { 'dashboard': 'Dashboard', 'add-product': 'Add Product', 'blockchain': 'Blockchain Ledger' };
    document.getElementById('page-title').innerText = titles[sectionId];

    // Hide search on non-dashboard
    document.getElementById('search-container').style.display = sectionId === 'dashboard' ? 'flex' : 'none';

    if (sectionId === 'blockchain') {
        loadBlockchain();
    }
}

// Load Products
async function loadProducts(reverse = false) {
    isReverse = reverse;
    const endpoint = reverse ? '/products/reverse' : '/products';
    try {
        const res = await fetch(`${API_URL}${endpoint}`);
        const products = await res.json();
        renderTable(products);
    } catch (err) {
        console.error(err);
        showToast('Error loading products', 'error');
    }
}

// Search Products
async function searchProducts(query) {
    try {
        const res = await fetch(`${API_URL}/search?query=${encodeURIComponent(query)}`);
        const products = await res.json();
        renderTable(products);
    } catch (err) {
        console.error(err);
    }
}

// Render Table
function renderTable(products) {
    const tbody = document.getElementById('product-list');
    tbody.innerHTML = '';

    if (!products || products.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" style="text-align:center; color: var(--text-secondary)">No products found</td></tr>';
        return;
    }

    products.forEach(p => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td>#${p.id}</td>
            <td><strong>${p.name}</strong></td>
            <td><span style="background:rgba(56, 189, 248, 0.1); color:#38bdf8; padding:4px 8px; border-radius:4px; font-size:0.8rem">${p.category}</span></td>
            <td>Rs. ${parseFloat(p.price).toFixed(2)}</td>
            <td>${p.stock}</td>
            <td>
                <button class="btn btn-secondary btn-sm" onclick="openEditModal(${p.id}, '${p.name}', ${p.price}, ${p.stock})">
                    <i class="fa-solid fa-edit"></i>
                </button>
                <button class="btn btn-danger btn-sm" onclick="deleteProduct(${p.id})">
                    <i class="fa-solid fa-trash"></i>
                </button>
            </td>
        `;
        tbody.appendChild(tr);
    });
}

// Actions
async function deleteProduct(id) {
    if (!confirm('Are you sure you want to delete this product?')) return;

    try {
        const res = await fetch(`${API_URL}/products/${id}`, { method: 'DELETE' });
        const result = await res.json();
        if (result.status === 'success') {
            showToast('Product deleted', 'success');
            loadProducts(isReverse);
        } else {
            showToast('Error deleting product', 'error');
        }
    } catch (err) {
        console.error(err);
    }
}

function openEditModal(id, name, price, stock) {
    document.getElementById('edit-id').value = id;
    document.getElementById('edit-name').value = name;
    document.getElementById('edit-price').value = price;
    document.getElementById('edit-stock').value = stock;
    document.getElementById('edit-modal').style.display = 'block';
}

function closeModal() {
    document.getElementById('edit-modal').style.display = 'none';
}

function toggleSortOrder() {
    loadProducts(!isReverse);
    const btn = document.getElementById('sort-btn');
    if (!isReverse) {
        btn.innerHTML = '<i class="fa-solid fa-sort"></i> Reverse View';
    } else {
        btn.innerHTML = '<i class="fa-solid fa-sort"></i> Standard View';
    }
}

// Blockchain Logic
async function loadBlockchain() {
    try {
        const res = await fetch(`${API_URL}/blockchain`);
        const chain = await res.json();
        renderBlockchain(chain);
    } catch (err) {
        console.error(err);
        showToast('Error loading blockchain', 'error');
    }
}

function renderBlockchain(chain) {
    const container = document.getElementById('blockchain-container');
    container.innerHTML = '';

    chain.forEach(block => {
        const card = document.createElement('div');
        card.className = 'block-card';
        card.innerHTML = `
            <div class="block-header">
                <span class="block-index">Block #${block.index}</span>
                <span class="block-time">${block.timestamp}</span>
            </div>
            <div class="block-data">
                <i class="fa-solid fa-file-contract"></i> ${block.data}
            </div>
            <div class="block-hash">
                <span class="hash-label">PREV:</span> ${block.prev_hash}<br>
                <span class="hash-label">HASH:</span> ${block.hash}
            </div>
        `;
        container.appendChild(card);
    });
}

async function verifyChain() {
    try {
        const res = await fetch(`${API_URL}/blockchain/verify`);
        const result = await res.json();
        if (result.status === 'valid') {
            showToast('Blockchain is VALID and Secure', 'success');
        } else {
            showToast('TAMPERING DETECTED: ' + result.message, 'error');
        }
    } catch (err) {
        console.error(err);
    }
}

// Toast Function
function showToast(message, type = 'success') {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${type}`;

    const icon = type === 'success' ? '<i class="fa-solid fa-check-circle"></i>' : '<i class="fa-solid fa-circle-exclamation"></i>';

    toast.innerHTML = `${icon} <span>${message}</span>`;

    container.appendChild(toast);

    // Auto remove
    setTimeout(() => {
        toast.style.animation = 'toastFadeOut 0.3s forwards';
        setTimeout(() => toast.remove(), 300);
    }, 4000);
}

// Close modal on outside click
window.onclick = function (event) {
    if (event.target == document.getElementById('edit-modal')) {
        closeModal();
    }
}
