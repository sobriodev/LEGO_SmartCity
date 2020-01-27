const legoApp = angular.module('LegoApp',[]);

legoApp.controller('LegoCtrl', ['$scope', function($scope) {
    $scope.lego = LegoData;

    $scope.imgPath = './lego_img/';
    $scope.imgDir = function (id) {
        return $scope.imgPath + id + '.jpg';
    };

    $scope.toggleLight = function (id) {
        console.log(id);
    }

}]);