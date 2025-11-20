import { API_URL } from '@/config/app'
import { useAuthStore } from '@/stores/authStore'
import router from '@/router'

type HTTPMethod = 'GET' | 'POST' | 'PATCH' | 'DELETE'

interface RequestOptions {
  responseType?: 'json' | 'text'
}

async function request<T>(
  path: string,
  method: HTTPMethod,
  params = {},
  options: RequestOptions = {}
): Promise<T> {
  const { responseType = 'json' } = options
  const authStore = useAuthStore()
  const token = authStore.getToken()

  const headers: HeadersInit = {
    'Content-Type': method === 'PATCH' ? 'application/merge-patch+json' : 'application/json; charset=utf-8',
    ...(token ? { Authorization: `Bearer ${token}` } : {}),
  }

  let url = `${API_URL}${path}`
  let body: BodyInit | undefined = undefined

  if (method === 'GET' || method === 'DELETE') {
    const query = new URLSearchParams(params).toString()
    if (query) {
      url += `?${query}`
    }
  } else {
    body = JSON.stringify(params)
  }

  const response = await fetch(url, {
    method,
    headers,
    ...(body ? { body } : {}),
  })

  // Handle 401 Unauthorized or 403 Forbidden
  if (response.status === 401 || response.status === 403) {
    authStore.clearToken()
    router.push({
      path: '/login',
      query: { reason: 'unauthorized' },
    })
    throw new Error('Session expired or unauthorized')
  }

  // Handle text responses
  if (responseType === 'text') {
    if (!response.ok) {
      throw new Error(`${method} ${path} failed: ${response.statusText}`)
    }
    return response.text() as Promise<T>
  }

  // Handle JSON responses
  const contentType = response.headers.get('Content-Type')
  if (contentType?.includes('json')) {
    const data = await response.json()

    if (!response.ok && (data.error || data.status !== 200)) {
      throw new Error(data.error || data.detail)
    }

    return data
  }

  if (response.status === 204) {
    return { success: true } as T
  }

  throw new Error(`${method} ${path} failed: ${response.statusText}`)
}

export { request }
