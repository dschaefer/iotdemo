import React from 'react';
import ReactDOM from 'react-dom';

export class SensorButton extends React.Component {
	render() {
		return (
			<div>
				<button type="button" className="btn btn-success">Sensor 17</button><br />
			</div>
		);
	}
}

export class Sensors extends React.Component {
	render() {
		return (
			<div>
				<p>Sensors:</p>
				<SensorButton />
				<SensorButton />
			</div>
		);
	}
}
ReactDOM.render(
		<div>
			<h3>Hello from Doug's IoT Demo</h3>
			<Sensors />
		</div>,
		document.getElementById("main")
);
