var extend = require('extend');

/**
 * Generate dot code via a fluent style
 * 
 * Dot code is compatible with Graphviz. Currently only
 * a limited language is supported for the use-case of 
 * generating a simple tree diagram.
 *
 * For reference, see www.graphviz.org/doc/info/lang.html 
 */
function DotCode()
{
	var listing = [];
	var nodeStyles = {};
	var edgeStyles = {};

	/**
	 * Checks if an object is empty
	 *
	 * Ref: http://stackoverflow.com/questions/679915/how-do-i-test-for-an-empty-javascript-object
	 * 
	 * @param  {object}  obj Object to check
	 * @return {Boolean}     True only if object is empty
	 */
	function isEmptyObject(obj) {
		return Object.keys(obj).length === 0 && obj.constructor === Object;
	}

	/**
	 * Generate property list code
	 * @param  {object} props Map of properties
	 * @return {string}       Property list code
	 */
	function generatePropList(props) {
		if(!props)
			return '';

		var args = [];
		for(var d in props) {
			args.push(d + '="' + props[d] + '"');
		}

		return ' [' + args.join(', ') + ']';
	}

	/**
	 * Add a digraph
	 * @param  {string}   name     Name of digraph
	 * @param  {Function} callback Callback that receives a DotCode object that can be used to add code to digraph
	 * @return {DotCode}           Current instance
	 */
	this.digraph = function(name, callback) {
		var d = {
			type: 'digraph',
			name: name,
			tree: new DotCode()
		};
		callback(d.tree);
		listing.push(d);
		return this;
	}

	/**
	 * Add a node
	 *
	 * First argument is optional. If the user just provides a
	 * property list, then it is used to set the properties of
	 * all the nodes in general.
	 * 
	 * @param  {string} id   Name of node
	 * @param  {map} prop    Property map for node
	 * @return {DotCode}     Current instance
	 */
	this.node = function(id, prop) {
		if(arguments.length == 1 && typeof id != 'string') {
			return this.nodeStyle(id);
		}

		listing.push({
			type: 'node',
			id: id,
			properties: prop
		});
		return this;
	}

	/**
	 * Add an edge
	 *
	 * First argument is optional. If the user just provides a
	 * property list, then it is used to set the properties of
	 * all the edges in general.
	 * 
	 * @param  {string} from Name of start node
	 * @param  {string} to   Name of end node
	 * @param  {map} prop    Property map for edge
	 * @return {DotCode}     Current instance
	 */
	this.edge = function(from, to, prop) {
		if(arguments.length == 1 && typeof from != 'string') {
			return this.edgeStyle(from);
		}

		listing.push({
			type: 'edge',
			from: from,
			to: to,
			properties: prop
		});
		return this;
	}

	/**
	 * Extend node properties for all nodes with given property map.
	 * @param  {map} prop Property map
	 * @return {DotCode}  Current Instance
	 */
	this.nodeStyle = function(prop) {
		extend(nodeStyles, prop);
		return this;
	}

	/**
	 * Extend edge properties for all edges with given property map.
	 * @param  {map} prop Property map
	 * @return {DotCode}  Current Instance
	 */
	this.edgeStyle = function(prop) {
		extend(edgeStyles, prop);
		return this;
	}

	/**
	 * Generate the dot code for this graph
	 * @return {string} Dot code generated as string
	 */
	this.toCode = function() {
		var lines = [];

		if(!isEmptyObject(nodeStyles)) {
			lines.push('node ' + generatePropList(nodeStyles) + ';');
		}

		if(!isEmptyObject(edgeStyles)) {
			lines.push('edge ' + generatePropList(edgeStyles) + ';');
		}

		for (var i = 0; i < listing.length; i++) {
			var x = listing[i];
			if(x.type == "digraph") {
				lines.push("digraph " + x.name + " {");
				lines.push(x.tree.toCode());
				lines.push("}");
			} else if(x.type == "node") {
				lines.push(x.id + generatePropList(x.properties) + ';')
			} else if(x.type == "edge") {
				lines.push(x.from + ' -> ' + x.to + generatePropList(x.properties) + ';');
			}
		}

		return lines.join("\n");
	}

}

module.exports = DotCode;