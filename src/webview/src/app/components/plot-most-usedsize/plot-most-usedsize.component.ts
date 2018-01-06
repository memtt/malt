import { Component, OnInit, OnChanges, ViewChild, ElementRef, Input, ViewEncapsulation } from '@angular/core';
import { HelperService } from '../../common/helper.service';
import * as d3 from 'd3';
import * as tip from 'd3-tip';

@Component({
	selector: 'app-plot-most-usedsize',
	templateUrl: './plot-most-usedsize.component.html',
	styleUrls: ['./plot-most-usedsize.component.css'],
	encapsulation: ViewEncapsulation.None
})
export class PlotMostUsedsizeComponent implements OnInit {
	@ViewChild('chart') private chartContainer: ElementRef;
	@Input() private data: Array<any>;

	constructor(private helper:HelperService) { }

	ngOnInit() {
		this.setupMostUsedChart();
	}

	setupMostUsedChart()
	{
		var cur = this;
		var element = this.chartContainer.nativeElement;
		var svgWidth = parseInt(d3.select(element).select('svg').style('width'));
		var svgHeight = parseInt(d3.select(element).select('svg').style('height'));

		var margin = {top: 20, right: 20, bottom: 30, left: 60},
			width = svgWidth - margin.left - margin.right,
			height = svgHeight - margin.top - margin.bottom;

		var formatPercent = d3.format(".0");

		var x = d3.scale.ordinal()
			.rangeRoundBands([0, width], .31, 1);

		var y = d3.scale.linear()
			.range([height, 0]);

		var xAxis = d3.svg.axis()
			.scale(x)
			.orient("bottom")
			.tickFormat(function(d) { if (d == 0) { return 'others'; } else { return cur.helper.humanReadable(d,1,'',false);} });

		var yAxis = d3.svg.axis()
			.scale(y)
			.orient("left")
			.tickFormat(function(d) { return cur.helper.humanReadable(d,1,'',false); });

		var yAxisLines = d3.svg.axis()
			.scale(y)
			.orient("left")
			.tickFormat("");

		var svg = d3.select(element).select('svg')
			.attr("width", width + margin.left + margin.right)
			.attr("height", height + margin.top + margin.bottom)
		.append("g")
			.attr("transform", "translate(" + margin.left + "," + margin.top + ")");

		var tip2 = tip()
			.attr('class', 'd3-tip')
			.offset([-10, 0])
			.html(function(d:any) {
				return "<strong>Frequency:</strong> <span style='color:red'>" + cur.helper.humanReadable(+(d.frequency),1,'',false) + "</span>";
			});

		//svg.call(tip);

		//setup data
		var formattedData = [];
		var cnt = 0;
		var max = 0;
		var tmp = [];
		var sum = 0;

		for(var i in this.data)
		{
			tmp.push({size:+i,frequency:+this.data[i]});
			sum+=+this.data[i];
			if (this.data[i] > max)
				max = this.data[i];
		}

		tmp = tmp.sort(function(a,b) {return b.frequency - a.frequency;})

		var others = 0;
		for(var i in tmp)
		{
			//alert(i + " -> " + data[i]);
			if (tmp[i].frequency > max/1000 && cnt < 50)
			{
				formattedData.push(tmp[i]);
				cnt ++;
			} else {
				others += +tmp[i].frequency;
			}
		}
		formattedData.push({size:0,frequency:+others});
		formattedData = formattedData.sort(function(a,b) {return b.frequency - a.frequency;})

		//alert(JSON.stringify(data));

		x.domain(formattedData.map(function(d) { return d.size; }));
		y.domain([0, d3.max(formattedData, function(d) { return d.frequency; })]);

		svg.append("g")
			.attr("class", "x axis")
			.attr("transform", "translate(0," + height + ")")
			.call(xAxis);

		/*svg.append("g")
			.attr("class", "grid")
			.call(yAxisLines
				.tickSize(-width, 0, 0)
				.tickFormat("")
			);*/

		svg.append("g")
			.attr("class", "y axis")
			.call(yAxis)
			.append("text")
			.attr("transform", "rotate(-90)")
			.attr("y", 6)
			.attr("dy", ".71em")
			.style("text-anchor", "end")
			.text("Frequency");

		svg.selectAll(".bar")
			.data(formattedData)
			.enter().append("rect")
			.attr("class", "bar")
			.attr("x", function(d) { return x(d.size); })
			.attr("width", x.rangeBand())
			.attr("y", function(d) { return y(d.frequency); })
			.attr("height", function(d) { return height - y(d.frequency); })
			.on('mouseover', tip2.show)
			.on('mouseout', tip2.hide);

		d3.select(element).select("input").on("change", change);

		var sortTimeout = setTimeout(function() {
			d3.select(element).select("input").property("checked", true).each(change);
		}, 500);

		function change() {
			clearTimeout(sortTimeout);

			// Copy-on-write since tweens are evaluated after a delay.
			var x0 = x.domain(formattedData.sort(this.checked
				? function(a, b) { return b.frequency - a.frequency; }
				: function(a, b) { return d3.ascending(a.size, b.size); })
				.map(function(d) { return d.size; }))
				.copy();

			var transition = svg.transition().duration(750),
				delay = function(d, i) { return i * 50; };

			transition.selectAll(".bar")
				.delay(delay)
				.attr("x", function(d) { return x0(d.size); });

			transition.select(".x.axis")
				.call(xAxis)
			.selectAll("g")
				.delay(delay);
		}
	}


}
