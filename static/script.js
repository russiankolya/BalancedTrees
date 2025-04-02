document.addEventListener('DOMContentLoaded', function() {
    const treeTypeSelect = document.getElementById('tree-type');
    const createTreeBtn = document.getElementById('create-tree-btn');
    const treeList = document.getElementById('tree-list');
    const treeIdSpan = document.getElementById('tree-id');
    const nodeValueInput = document.getElementById('node-value');
    const insertBtn = document.getElementById('insert-btn');
    const removeBtn = document.getElementById('remove-btn');
    const deleteTreeBtn = document.getElementById('delete-tree-btn');
    const treeVisualization = document.getElementById('tree-visualization');
    
    let selectedTreeId = null;
    
    loadTrees();
    
    createTreeBtn.addEventListener('click', createTree);
    insertBtn.addEventListener('click', insertNode);
    removeBtn.addEventListener('click', removeNode);
    deleteTreeBtn.addEventListener('click', deleteTree);
    
    function loadTrees() {
        fetch('/trees')
            .then(response => response.json())
            .then(trees => {
                treeList.innerHTML = '';
                if (trees.length === 0) {
                    treeList.innerHTML = 'No trees created yet';
                    return;
                }
                
                trees.forEach(tree => {
                    const treeElement = document.createElement('div');
                    treeElement.className = 'tree-item';
                    treeElement.textContent = `${tree.type} (${tree.id})`;
                    treeElement.dataset.id = tree.id;
                    treeElement.addEventListener('click', () => selectTree(tree.id));
                    treeList.appendChild(treeElement);
                });
            })
            .catch(error => console.error('Error loading trees:', error));
    }
    
    function createTree() {
        const treeType = treeTypeSelect.value;
        
        fetch('/trees', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ type: treeType })
        })
        .then(response => response.json())
        .then(data => {
            loadTrees();
            selectTree(data.id);
        })
        .catch(error => console.error('Error creating tree:', error));
    }
    
    function selectTree(treeId) {
        selectedTreeId = treeId;
        treeIdSpan.textContent = treeId;
        
        const treeItems = document.querySelectorAll('.tree-item');
        treeItems.forEach(item => {
            if (item.dataset.id === treeId) {
                item.classList.add('selected');
            } else {
                item.classList.remove('selected');
            }
        });
        
        fetchTreeData(treeId);
    }
    
    function fetchTreeData(treeId) {
        fetch(`/trees/${treeId}`)
            .then(response => response.json())
            .then(treeData => {
                visualizeTree(treeData);
            })
            .catch(error => {
                console.error('Error fetching tree data:', error);
                treeVisualization.innerHTML = 'Error loading tree data';
            });
    }
    
    function insertNode() {
        if (!selectedTreeId) {
            alert('Please select a tree first');
            return;
        }
        
        const value = parseInt(nodeValueInput.value);
        if (isNaN(value)) {
            alert('Please enter a valid number');
            return;
        }
        
        fetch(`/trees/${selectedTreeId}/insert`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ value: value })
        })
        .then(response => response.json())
        .then(() => {
            nodeValueInput.value = '';
            fetchTreeData(selectedTreeId);
        })
        .catch(error => console.error('Error inserting node:', error));
    }
    
    function removeNode() {
        if (!selectedTreeId) {
            alert('Please select a tree first');
            return;
        }
        
        const value = parseInt(nodeValueInput.value);
        if (isNaN(value)) {
            alert('Please enter a valid number');
            return;
        }
        
        fetch(`/trees/${selectedTreeId}/remove`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ value: value })
        })
        .then(response => response.json())
        .then(() => {
            nodeValueInput.value = '';
            fetchTreeData(selectedTreeId);
        })
        .catch(error => console.error('Error removing node:', error));
    }
    
    function deleteTree() {
        if (!selectedTreeId) {
            alert('Please select a tree first');
            return;
        }
        
        if (confirm('Are you sure you want to delete this tree?')) {
            fetch(`/trees/${selectedTreeId}`, {
                method: 'DELETE'
            })
            .then(response => response.json())
            .then(() => {
                selectedTreeId = null;
                treeIdSpan.textContent = 'None';
                treeVisualization.innerHTML = 'Select a tree to visualize it';
                loadTrees();
            })
            .catch(error => console.error('Error deleting tree:', error));
        }
    }
    
    function visualizeTree(treeData) {
        treeVisualization.innerHTML = '';
        
        if (!treeData || !treeData.nodes || treeData.nodes.length === 0) {
            treeVisualization.innerHTML = 'Tree is empty';
            return;
        }
        
        const treeContainer = document.createElement('div');
        treeContainer.className = 'tree-container';
        
        const nodes = treeData.nodes;
        
        const treeRoot = buildTreeNode(0, nodes);
        treeContainer.appendChild(treeRoot);
        treeVisualization.appendChild(treeContainer);
    }
    
    function buildTreeNode(nodeIndex, nodes) {
        const node = nodes[nodeIndex];
        const nodeElement = document.createElement('div');
        nodeElement.className = 'tree-node';
        
        const valueElement = document.createElement('div');
        valueElement.className = 'node-value';
        valueElement.textContent = node.key;
        nodeElement.appendChild(valueElement);
        
        const hasLeftChild = node.left !== -1;
        const hasRightChild = node.right !== -1;
        
        if (hasLeftChild || hasRightChild) {
            const childrenElement = document.createElement('div');
            childrenElement.className = 'node-children';
            
            const leftSide = document.createElement('div');
            leftSide.className = 'child-branch left-branch';
            
            const rightSide = document.createElement('div');
            rightSide.className = 'child-branch right-branch';
            
            if (hasLeftChild) {
                const leftChildNode = buildTreeNode(node.left, nodes);
                leftSide.appendChild(leftChildNode);
            }
            
            if (hasRightChild) {
                const rightChildNode = buildTreeNode(node.right, nodes);
                rightSide.appendChild(rightChildNode);
            }
            
            childrenElement.appendChild(leftSide);
            childrenElement.appendChild(rightSide);
            nodeElement.appendChild(childrenElement);
        }
        
        return nodeElement;
    }
});
