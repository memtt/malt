/**********************************************************************/
function MattHelper()
{
}

/**********************************************************************/
/**
 * Help to padd interger value with 0
 * @param value Define the value to padd
 * @param size Define the final size of the string.
**/
MattHelper.prototype.zeroFill = function(value,size)
{
	var cur = value;
	var pad = '';
	
	//padd
	while (size > 1)
	{
		cur = cur / 10;
		size--;
		if (cur < 1)
			pad+='0';
	}
	
	//final string
	var res = pad + value;

	//errors
	if (res.length != size)
		console.log('Invalid size of padded string !');

	//return
	return res;
}

/**********************************************************************/
/**
 * Help to convert ticks timings to hh:mm:ss.
 * @param t time to convert
 * @param ticksPerSec Ref to convert ticks to seconds.
**/
MattHelper.prototype.ticksToHourMinSec = function(t,ticksPerSec)
{
	//calc sec
	var tot = Math.round(t / ticksPerSec);
	
	//calc min
	var sec = tot % 60;
	var min = Math.floor(tot / 60);
	var hour = Math.floor(tot / 60 / 60);
	var days = Math.floor(tot / 60 / 60 / 24);

	//gen string
	var res = '';
	if (tot >= 60*60*24) res += this.zeroFill(days,2) + '-';
	res += this.zeroFill(hour,2) + ':';
	res += this.zeroFill(min,2) + ':';
	res += this.zeroFill(sec,2);

	return res;
}

/**********************************************************************/
mattHelper = new MattHelper();
