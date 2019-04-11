/**
 * A C++ Method Declaration parser
 * 
 * Breaks a declaration into its components, such as
 * method name, class, namespace, arguments, type parameters
 * and return type. Supports a limited subset of declarations
 * only.
 */
(function() {
	/**
	 * Provide fallback because endsWidth() is not available without extra node options
	 * for old nodejs still in use on old redhat release.
	 * https://stackoverflow.com/questions/27599537/string-prototype-in-node-v0-10-25-does-not-have-endswith
	 */
	String.prototype.endsWith = String.prototype.endsWith || function(str){
		return new RegExp(str + "$").test(str);
	}

	/**
	 * Get the type parameter start index for a CPP prototype string
	 * @param  {string} func Prototype declaration string
	 * @return {int}         Start index
	 */
	function getParameterStartIndex(func) {
		var gtCount = 1, i = func.length - 2;

		while(gtCount > 0 && i >= 0) {
			if(func[i] == '>') {
				gtCount++;
			} else if(func[i] == '<') {
				gtCount--;
			}
			i--;
		}

		if(gtCount != 0)
			return null;

		return i + 1;
	}

	var CppDeclParser = {
		/**
		 * Get description for a C++ prototype declaration
		 * @param  {string} func Prototype Declaration string
		 * @return {object}      Description of the function
		 */
		parseCppPrototype: function(func) {
			var funcDescription = {
				full: null,
				arguments: null,
				className: null,
				funcName: null,
				funcParameters: null,
				classParameters: null,
				returnType: null,
				namespace: null,
				isConst: false
			};

			var func = funcDescription.full = func.trim();

			// Check if prototype is actually a missing and this is a memory description
			if(func.endsWith(']')) {
				funcDescription.funcName = func;
				return funcDescription;
			}

			// Check if the function is constant
			if(func.endsWith('const')) {
				func = func.substr(0, func.length - 'const.'.length).trim();
				funcDescription.isConst = true;
			}

			var doesHaveBraces = (func[func.length-1] == ')');

			// Get args list
			if(doesHaveBraces) {
				funcDescription.arguments = func.substring(func.lastIndexOf('(')+1, func.length-1);
				func = func.substring(0, func.lastIndexOf('(')).trim();
				if(!funcDescription.arguments.length) {
					funcDescription.arguments = null;
				}
			}

			var isFunctionParameterized = (func[func.length-1] == '>');

			// Get function type parameters
			if(isFunctionParameterized) {
				var parameterStartIndex = getParameterStartIndex(func);
				funcDescription.funcParameters = func.substring(parameterStartIndex+1, func.length-1);
				func = func.substring(0, parameterStartIndex)
			}

			var doubleColonPosition = func.lastIndexOf('::');
			var hasClass = (doubleColonPosition != -1) && (func.substr(doubleColonPosition).lastIndexOf(' ') == -1);

			// Get function name if there is class
			if(hasClass) {
				funcDescription.funcName = func.substr(doubleColonPosition + 2);
				func = func.substring(0, doubleColonPosition);
			}

			var isClassParameterized = (func[func.length-1] == '>');

			// Get the class type parameters if there is class
			if(hasClass && isClassParameterized) {
				var parameterStartIndex = getParameterStartIndex(func);
				funcDescription.classParameters = func.substring(parameterStartIndex+1, func.length-1);
				func = func.substring(0, parameterStartIndex)
			}

			var lastSpacePosition = func.lastIndexOf(' ')
			var hasReturnValue = (lastSpacePosition != -1);

			// Get class name and namespace
			if(hasClass) {
				var className = func;

				if(hasReturnValue) {
					className = func.substr(lastSpacePosition + 1);
					func = func.substring(0, lastSpacePosition);
				}

				// Is namespaced?
				if(className.lastIndexOf('::') != -1) {
					funcDescription.namespace = className.substring(0, className.lastIndexOf('::'));
					className = className.substr(className.lastIndexOf('::') + 2);
				}

				funcDescription.className = className;
			}

			// Get function name if no class
			if(!hasClass) {
				if(hasReturnValue) {
					funcDescription.funcName = func.substr(lastSpacePosition);
					func = func.substring(0, lastSpacePosition);
				} else {
					funcDescription.funcName = func;
				}
			}

			// Get return type if any
			if(hasReturnValue) {
				funcDescription.returnType = func.trim();
			}

			return funcDescription;
		},

		/**
		 * Get a short name for a method from its description object
		 * @param  {object} funcDescription Description object
		 * @return {string}                 Short name
		 */
		getShortName: function(funcDescription) {
			var name = '';

			if(funcDescription.namespace) {
				name += funcDescription.namespace + '::';
			}

			if(funcDescription.className) {
				name += funcDescription.className;
				if(funcDescription.classParameters) {
					name += '<...>';
				}
				name += '::'
			}


			name += funcDescription.funcName;

			if(funcDescription.funcParameters) {
				name += '<...>';
			}

			if(funcDescription.arguments) {
				name += '(...)';
			} else if(!funcDescription.funcName.endsWith(']')) {
				name += '()';
			}

			return funcDescription.full.length > name.length ? name : funcDescription.full;
		}
	}

	if(typeof exports !== undefined) {
		module.exports = CppDeclParser;
	} else {
		window.CppDeclParser = CppDeclParser;
	}
})();