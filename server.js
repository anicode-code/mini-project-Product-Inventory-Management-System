const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const { execFile } = require('child_process');
const path = require('path');

const app = express();
const PORT = 3000;
const BINARY_PATH = path.join(__dirname, 'logic');

app.use(cors());
app.use(bodyParser.json());
app.use(express.static('public'));

// Helper to execute C binary
const executeLogic = (args, res) => {
    execFile(BINARY_PATH, args, (error, stdout, stderr) => {
        if (error) {
            console.error('Error executing logic:', stderr);
            return res.status(500).json({ error: 'Internal Server Error', details: stderr });
        }
        try {
            // Attempt to find the JSON array/object in the output if there's noise
            const jsonStart = stdout.indexOf('{');
            const jsonStartArr = stdout.indexOf('[');
            let startIndex = 0;

            if (jsonStartArr !== -1 && (jsonStart === -1 || jsonStartArr < jsonStart)) startIndex = jsonStartArr;
            else if (jsonStart !== -1) startIndex = jsonStart;

            const cleanStdout = stdout.substring(startIndex);
            const data = JSON.parse(cleanStdout);
            res.json(data);
        } catch (e) {
            console.error('JSON Parse Error:', e, stdout);
            res.status(500).json({ error: 'Invalid response from backend', raw: stdout });
        }
    });
};

// API Routes

// Get all products
app.get('/api/products', (req, res) => {
    executeLogic(['read'], res);
});

// Get all products reversed
app.get('/api/products/reverse', (req, res) => {
    executeLogic(['read_reverse'], res);
});

// Add product
app.post('/api/products', (req, res) => {
    const { id, name, category, price, stock } = req.body;
    if (!id || !name || !category) {
        return res.status(400).json({ error: 'Missing required fields' });
    }
    executeLogic(['add', id, name, category, price, stock], res);
});

// Update product
app.put('/api/products/:id', (req, res) => {
    const { id } = req.params;
    const { price, stock } = req.body;
    executeLogic(['update', id, price, stock], res);
});

// Delete product
app.delete('/api/products/:id', (req, res) => {
    const { id } = req.params;
    executeLogic(['delete', id], res);
});

// Search products
app.get('/api/search', (req, res) => {
    const { query } = req.query;
    executeLogic(['search', query], res);
});

// Blockchain Routes
app.get('/api/blockchain', (req, res) => {
    executeLogic(['view_chain'], res);
});

app.get('/api/blockchain/verify', (req, res) => {
    executeLogic(['verify_chain'], res);
});

app.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
});
