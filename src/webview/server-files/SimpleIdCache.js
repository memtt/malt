/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.3.0
*    DATE     : 04/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/webview/server-files/SimpleIdCache.js
*-----------------------------------------------------------
*    AUTHOR   : Mehdi Raza Jaffery (CERN) - 2016
*    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2025
***********************************************************/
/**
 * SimpleIdCache class
 *
 * Tracks objects by assigning unique numeric ids to them.
 */
//--PORTED IN C++ VERSION--
function SimpleIdCache()
{
	this.cache = [];
	this.id = 1;

	/**
	 * Checks if the object is already in the cache
	 * @param  {string} value The object to check
	 * @return {boolean}      True if value is in cache
	 */
	this.exists = function(value) {
		if(this.cache[value] === undefined)
			return false;
		return true;
	}

	/**
	 * Put an object into the cache and returns its unique id.
	 *
	 * If the object already exists in the cache, just
	 * returns the id of the object.
	 *
	 * @param  {string} value Value to put into the cache
	 * @return {int}          Unique id for this value
	 */
	this.put = function(value) {
		if(this.exists(value))
			return this.cache[value];
		this.cache[value] = this.id++;
		return this.cache[value];
	}

	/**
	 * Get the unique id for an object.
	 *
	 * If the object is not in the cache, returns false.
	 *
	 * @param  {string} value Value to check in the cache
	 * @return {int|boolean}  Returns the unqiue id or
	 *                        false if value is not found.
	 */
	this.get = function(value) {
		if(!this.exists(value))
			return false;
		return this.cache[value];
	}

	return this;
}

module.exports = SimpleIdCache;
