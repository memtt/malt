import { MattWebviewPage } from './app.po';

describe('matt-webview App', function() {
  let page: MattWebviewPage;

  beforeEach(() => {
    page = new MattWebviewPage();
  });

  it('should display message saying app works', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('app works!');
  });
});
