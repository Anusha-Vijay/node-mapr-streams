import React, { Component } from 'react';
import './App.css';

function isSearched(searchTerm) {
  return function(item) {
    return item.key.toLowerCase().includes(searchTerm.toLowerCase());
  }
}
/**
 * Given A stream
 * Be able to get the contents of data of the stream and reload more on refresh or time.
 * Write producer and consumer on browser.
 * Link to external data set and produce and consume to stream
 */
class App extends Component {

  constructor(props) {
    super(props);

    this.state = {
      result: null,
      searchTerm: "",
    };
    this.setSearchTopStories = this.setSearchTopStories.bind(this);
    this.fetchSearchTopStories = this.fetchSearchTopStories.bind(this);
    this.onSearchChange = this.onSearchChange.bind(this);
    this.onDismiss = this.onDismiss.bind(this);
  }

  setSearchTopStories(result) {
    console.log(`setSearchTopStories ==> ${result}`);
    this.setState({ result });
  }

  fetchSearchTopStories(searchTerm) {
    fetch(`http://10.10.10.220:3100/consume`)
    .then(response => response.json())
    //.then(result => console.log(`==> ${result}`))
    .then(result => this.setSearchTopStories(result))
    .catch(e => e);
  }
    
  componentDidMount() {
    const { searchTerm } = this.state;
    console.log("componentDidMount");
    this.fetchSearchTopStories(searchTerm);
    console.log("componentDidMount done!");
  }

  onSearchChange(event) {
    this.setState({ searchTerm: event.target.value });
  }

  onDismiss(id) {
    const isNotId = item => item.objectID !== id;
    const updatedList = this.state.result.filter(isNotId);
    this.setState({ result: updatedList });
  }

  render() {
    console.log(`render ==> ${this.state}`);

    const { searchTerm, result } = this.state;
    if (!result) { return null; }

    const head2 = 'Welcome to MStreams React Example';
    return (
      <div className="page">
        <div className="interactions">
            <h2>{head2}</h2> 
            <Search value={searchTerm} onChange={this.onSearchChange}>Search</Search>
            <Table list={result} pattern={searchTerm} onDismiss={this.onDismiss}/>
        </div>
      </div>
    );
  }
}

class Search extends Component {
  render() {
    const { value, onChange, children } = this.props;
      return (
        <form>
          {children} <input type="text" value={value} onChange={onChange} />
        </form>
      );
    }
  }

class Table extends Component {
  
  render() {
    const fp = "item.payload.\"author\\";
    const { list, pattern, onDismiss } = this.props;
        return (
          <div className="table">
          {
            list.filter(isSearched(pattern)).map(
              item =>
                <div key={item.key} className="table-row">
                  <span style={{ width: '45%' }}>
                    <a href={item.key}>{fp}</a>
                  </span>
                  <span style={{ width: '45%' }}>
                    {fp}
                  </span>
                  {/* <span style={{ width: '10%' }}>
                    {item.num_comments}
                  </span>
                  <span style={{ width: '10%' }}>
                    {item.points}
                  </span> */
                  console.log("item:" + JSON.stringify(item, null, 4))
                  }
                  <span style={{ width: '10%' }}>
                    <button onClick={() => onDismiss(item.key)} type="button" className="button-inline">Dismiss</button>
                  </span>
              </div>
          )}
      </div>
      );
  }
}


export default App;