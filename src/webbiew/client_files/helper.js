/** Short helper to convert values to human readable format **/
var MATT_POWER = ['&nbsp;','K','M','G','T','P'];
function mattHumanReadableValue(value,unit)
{
	var power = 0;
	while (value >= 1000)
	{
		power++;
		value /= 1000;
	}

	return Math.round(value) + " " + MATT_POWER[power] + unit;
}
