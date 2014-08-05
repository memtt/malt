function MattPageAllocSizeDistr()
{
	//declare module to manage matt home page
	var mattModule = angular.module('matt.page.allocSizeDistr',[]);
	
	//main controler of the page
	var pageCtrl = mattCtrl.controller('matt.page.allocSizeDistr.ctrl',['$scope','$http',function($scope,$http) {
		//fetch summaryData
		$http.get('/size-map.json').success(function(data) {
			$scope.sizeMap = data;
		});
	}]);
	
	//directive for graphs
	pageCtrl.directive('mattsizedistrchart',function() {
		return {
			restrict: 'EA',
			template: '<div><svg/></div>',
			replace: true,
			scope: {
				data:'=',
			},
			link: function ($scope) {
				$scope.$watch('data', function(data){
					if ($scope.data != undefined && $scope.elementID != undefined)
						cur.buildChart('#mattsizedistrchart' + $scope.elementID + ' svg',$scope.data);
				});
			},
			controller: ['$scope', '$element', '$attrs', function($scope, $element, $attrs){
				if ($scope.elementID == undefined)
				{
					//setup element ID
					$scope.elementID = svgId++;
					angular.element($element).attr('id', 'mattsizedistrchart'+$scope.elementID );
				}
				
				if ($scope.height != undefined)
					d3.select('#mattsizedistrchart' + $scope.elementID + ' svg').attr('height',$scope.height);
			}]
		}
	});
}

//init and export
var mattAllocSizeDistr = new MattPageAllocSizeDistr();
