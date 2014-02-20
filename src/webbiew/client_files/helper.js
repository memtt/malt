function MattHelper()
{
	var MATT_POWER = ['&nbsp;','K','M','G','T','P'];

	/** Short helper to convert values to human readable format **/	
	this.humanReadableValue = function(value,unit)
	{
		var power = 0;
		while (value >= 1000)
		{
			power++;
			value /= 1000;
		}

		return value.toFixed(1) + " " + MATT_POWER[power] + unit;
	}
	
	/** Help to check values **/
	this.assert = function(condition,message)
	{
		if (!condition)
			throw Error("Assert failed" + (typeof message !== "undefined" ? ": " + message : ""));
	}
}

//setup global instance
var mattHelper = new MattHelper();
