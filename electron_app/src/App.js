import './App.css';
import Map from './modules/map/map';
import ScanSelect from './modules/scan_select/scan_select';
import Header from './modules/header/header';

function App() {
  return (
    <div className="App">
      <Header></Header>
      <div className="app-main">
        <ScanSelect></ScanSelect>
        <Map></Map>
      </div>
    </div>
  );
}

export default App;
