import { WebviewPage } from './app.po';

describe('webview App', () => {
  let page: WebviewPage;

  beforeEach(() => {
    page = new WebviewPage();
  });

  it('should display message saying app works', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('app works!');
  });
});
